/*
ENVIRONMENT: LINUX g++ 
gcc version : 9.3.0
Command To compile the source code : g++ 180101055_Assign2_assembler.cpp
Command to execute : ./a.out
*/

#include <bits/stdc++.h>

using namespace std;

const int MAXS = 1024;
const int MAXW = 10;

const int ILEN = 1 ;
const int WLEN = 1 ;

const char *symtabFile = "symbolTable.txt";

//List of Registers
unordered_map <string, string> register_list = {
    {"A", "0"},
    {"X", "1"},
    {"B", "3"},
    {"S", "4"},
    {"T", "5"},
    {"F", "6"}
};

int ERROR_FLAG = 0;

int PROGLEN;         // store program length
string PROGNAME; // store program length
int STARTADDR;       // store program length

class op_code{
public:
    string op_addr;
    int format;
};

//Operation Code Table
unordered_map <string, op_code> OPTAB;

void load_OPTAB(){
    OPTAB["LDA"] = {"00",3};
    OPTAB["LDX"] = {"04",3};
    OPTAB["LDL"] = {"08",3};
    OPTAB["LDB"] = {"68",3};
    OPTAB["LDT"] = {"74",3};
    OPTAB["STA"] = {"0C",3};
    OPTAB["STX"] = {"10",3};
    OPTAB["STL"] = {"14",3};
    OPTAB["LDCH"] = {"50",3};
    OPTAB["STCH"] = {"54",3};
    OPTAB["ADD"] = {"18",3};
    OPTAB["SUB"] = {"1C",3};
    OPTAB["MUL"] = {"20",3};
    OPTAB["DIV"] = {"24",3};
    OPTAB["COMP"] = {"28",3};
    OPTAB["COMPR"] = {"A0",2};
    OPTAB["CLEAR"] = {"B4",2};
    OPTAB["J"] = {"3C",3};
    OPTAB["JLT"] = {"38",3};
    OPTAB["JEQ"] = {"30",3};
    OPTAB["JGT"] = {"34",3};
    OPTAB["JSUB"] = {"48",3};
    OPTAB["RSUB"] = {"4C",3};
    OPTAB["TIX"] = {"2C",3};
    OPTAB["TIXR"] = {"B8",2};
    OPTAB["TD"] = {"E0",3};
    OPTAB["RD"] = {"D8",3};
    OPTAB["WD"] = {"DC",3};
}

op_code* search_optab(string opcode){
	if (OPTAB.find(opcode) != OPTAB.end()){
		return &OPTAB[opcode];
	}
	return NULL;
}

class symtab{
public:
    map<string, string> table;
    set<string> extref;
    set<string> extdef;
    int length;

    symtab(){}

    string search_symtab(string label){
        if (table.find(label) != table.end()){
            return table[label];
        }
        return "";
    }
    
    void insert_symtab(string label, int locctr){
        FILE *filesymtab = fopen(symtabFile, "a");
        char addr[MAXW];
        sprintf(addr, "%0X", locctr);
        table[label] = {addr};
        fprintf(filesymtab, "%s\t%s\n", addr, label.c_str());
        fclose(filesymtab);
    }
};

map<string, symtab *> symtab_list;
set<string> literal_pool;

struct modrec {
    int addr;
    int length;
    bool sign;
    string symbol;
};

int readLine(char *str, int start, char *words[], string delimiter){
	str = strtok(str, "\n");

	int size = start; 
	char *ptr = strtok(str, delimiter.c_str());

	while (ptr != NULL){
		words[size++] = ptr;
		ptr = strtok(NULL, delimiter.c_str());
	}

	return size;
}

int literalsOutput(int locctr, FILE *outputFile, symtab *base){
	while (true){
        if(literal_pool.size()==0){
            break;
        }
		string lit = *literal_pool.begin();

        FILE *filesymtab = fopen(symtabFile, "a");
        char addr[MAXW];
        sprintf(addr, "%0X", locctr);
        base->table[lit] = {addr};
        fprintf(filesymtab, "%s\t%s\n", addr, lit.c_str());
        fclose(filesymtab);

        fprintf(outputFile, "%04X\t*\t%s\n", locctr, lit.c_str());

		if(lit[1] != 'C'){
            locctr += (lit.length() - 4 + 1) / 2;
		}else{
			locctr += lit.length() - 4;
		}
        
        for(auto it:literal_pool){
            literal_pool.erase(it);
            break;
        }
		
	}
	return locctr;
}

int hexToIntSingleChar(char c){
    int n;
    if ((c <= '9') && (c >= '0')){
        n = c - '0';
    }else{
        n = c - 'A' + 10;
    }
    return n;
}

int hexToInt(string s){
    reverse(s.begin(), s.end());
    int j = 1;
    int ans = 0;
    for (int i = 0; i < s.length(); i++){
        ans += (hexToIntSingleChar(s[i]) * j);
        j = j * 16;
    }
    return ans;
}

FILE *progamFile, *intermediateFile, *objectFile, *listFile;

string OPCODE, LABEL="", OPERAND;
int LOCCTR;
bool extended;

int incrLOCCTR(){
    if (op_code *info = search_optab(OPCODE)){
        if (extended){
            LOCCTR = LOCCTR + (4 * ILEN);
        }else{
            LOCCTR = LOCCTR + (info->format * ILEN);
        }
    }else if (OPCODE=="WORD"){
        LOCCTR = LOCCTR + 3;
    }else if (OPCODE=="RESW"){
        LOCCTR = LOCCTR + (3 * stoi(OPERAND));
    }else if (OPCODE=="RESB"){
        LOCCTR = LOCCTR + stoi(OPERAND);
    }else if (OPCODE=="BYTE"){
        if (OPERAND[0] == 'C'){
            LOCCTR = LOCCTR + ((OPERAND.length())-3);
        }else if (OPERAND[0] == 'X'){
            LOCCTR = LOCCTR + (((OPERAND.length())-3+1)/2);
        }
    }else{
        return 1;
    }
    return 0;
}

void pass1(){

    fseek(progamFile, 0, SEEK_SET);

    int words;

    char *args[MAXW];

    char *line = NULL, temp[MAXS];
    size_t len = 0;

    // store addresses

    // read first line
    getline(&line, &len, progamFile);
    strcpy(temp, line);

    if(line[0] == ' '){
        words = readLine(temp, 1, args," ");
        LABEL = "";
        OPCODE = args[1];
    }else{
        words = readLine(temp, 0, args," ");
        LABEL = args[0];
        OPCODE = args[1];
    }

    if(strcmp(args[1], "START") == 0){
        OPERAND = args[2];
        STARTADDR = hexToInt(OPERAND);
        PROGNAME = LABEL;
        LOCCTR = STARTADDR;

        // write line to intermediate file
        fprintf(intermediateFile, "%04X\t%s", LOCCTR, line);

        // read next input line
        getline(&line, &len, progamFile);
        strcpy(temp, line);
        if (line[0] == ' '){
            words = readLine(temp, 1, args," ");
            //LABEL = NULL;
            OPCODE = args[1];
        }else{
            words = readLine(temp, 0, args," ");
            LABEL = args[0];
            OPCODE = args[1];
        }
    }else{
        LOCCTR = 0;
    }

    symtab *base = new symtab;
    symtab_list[PROGNAME] = base;
    while (OPCODE!="END"){
        if (line[0] != '.'){ // check if not a comment
            if (OPCODE=="CSECT"){
                LOCCTR = literalsOutput(LOCCTR, intermediateFile, base);
                base->length = LOCCTR;
                base = new symtab;
                symtab_list[(string)LABEL] = base;
                LOCCTR = 0;
            }else if (LABEL!=""){ // check if symbol in label
                if (base->search_symtab(LABEL)!=""){
                    cout<<"Error: Duplicate Symbol."<<LABEL<<endl;
                    exit(0);
                }else{
                    base->insert_symtab(LABEL, LOCCTR);
                }
            }

            extended = false;

            if (OPCODE[0] == '+'){
                extended = true;
                OPCODE = OPCODE.substr(1);
            }

            if (words > 2){
                OPERAND = args[2];
                if (OPERAND[0] == '='){
                    literal_pool.insert(OPERAND);
                }
            }

            // write line to intermediate file
            if (OPCODE=="EXTDEF"|| OPCODE=="EXTREF" || OPCODE=="LTORG"){
                fprintf(intermediateFile, "    \t%s", line);
            }else{
                fprintf(intermediateFile, "%04X\t%s", LOCCTR, line);
            }

            if(incrLOCCTR()==1){
                if (OPCODE=="EXTREF"){
                    char *operand = &OPERAND[0];
                    int size = readLine(operand, 0, args, ",");
                    int i=0;
                    while(i < size){
                        base->extref.insert(args[i]);
                        i++;
                    }
                }else if (OPCODE=="EXTDEF"){
                    OPERAND = args[2];
                    char *operand = &OPERAND[0];
                    int size = readLine(operand, 0, args, ",");
                    int i=0;
                    while(i < size){
                        base->extdef.insert(args[i]);
                        i++;
                    }
                }else if (OPCODE=="LTORG"){
                    LOCCTR = literalsOutput(LOCCTR, intermediateFile, base);
                }else if (OPCODE=="EQU"){
                }else if (OPCODE=="CSECT"){
                }else{
                    cout<<"ERROR: INVALID OPERATION CODE."<<endl;
                    exit(0);
                }
            }
            
        }else{
            fprintf(intermediateFile, "    \t%s", line);
        }

        getline(&line, &len, progamFile);
        strcpy(temp, line);

        if(line[0]==' '){
            words = readLine(temp, 1, args," ");
            LABEL = "";
            OPCODE = args[1];
        }else{
            words = readLine(temp, 0, args," ");
            LABEL = args[0];
            OPCODE = args[1];
        }
    }

    fprintf(intermediateFile, "    \t%s\n", line);

    LOCCTR = literalsOutput(LOCCTR, intermediateFile, base);

    PROGLEN = LOCCTR - STARTADDR;

    base->length = LOCCTR;

    cout<<"PASS 1 DONE"<<endl;
}

char* stringCopy(char* destination, const char* source){
    if (destination == NULL)
        return NULL;
 
    char *ptr = destination;
 
    while (*source != '\0'){
        *destination = *source;
        destination++;
        source++;
    }
 
    *destination = '\0';
 
    return ptr;
}

void pass2(){

    fseek(intermediateFile, 0, SEEK_SET);

    char *args[MAXW];
    int words;

    OPCODE="";
    LABEL="";
    OPERAND="";

    char *line = NULL, temp[MAXS], addr[MAXS];
    size_t len = 0;

    fscanf(intermediateFile, "%[^\t]s", addr);
    getline(&line, &len, intermediateFile);

    if (line[0] == ' '){
        stringCopy(temp, line);
        words = readLine(temp, 1, args," ");
        LABEL = "";
        OPCODE = args[1];
    }else{
        stringCopy(temp, line);
        words = readLine(temp, 0, args," ");
        LABEL = args[0];
        OPCODE = args[1];
    }

    bool first_sect = true;
    queue<modrec> modification_records;

    if (OPCODE=="START"){
        fprintf(listFile, "%s%s", addr, line);

        fscanf(intermediateFile, "%[^\t]s", addr);
        getline(&line, &len, intermediateFile);
        stringCopy(temp, line);
        if (line[0] == ' '){
            words = readLine(temp, 1, args," ");
            LABEL = "";
            OPCODE = args[1];
        }else{
            words = readLine(temp, 0, args," ");
            LABEL = args[0];
            OPCODE = args[1];
        }
    }

    symtab *base = symtab_list[PROGNAME];

    fprintf(objectFile, "H%-6s%06X%06X\n", PROGNAME.c_str(), STARTADDR, base->length);

    char firstaddr[MAXS] = "";
    sprintf(firstaddr, "%0X", STARTADDR);
    char record[MAXS] = "";

    while(true){
        char objcode[MAXS];
        stringCopy(objcode, "");
        if (line[1] != '.'){ 
            if (OPCODE=="CSECT"){
                base = symtab_list[LABEL];

                if (strlen(record) > 0){
                    fprintf(objectFile, "T%06X%02X%s\n", hexToInt(firstaddr),(int)strlen(record) / 2, record);
                    stringCopy(record, "");
                }

                while (!modification_records.empty()){
                    modrec rec = modification_records.front();
                    modification_records.pop();
                    fprintf(objectFile, "M%06X%02X%s%s\n", rec.addr, rec.length, rec.sign ? "+" : "-", rec.symbol.c_str());
                }

                fprintf(objectFile, "E");
                if (first_sect){
                    fprintf(objectFile, "E%06X", 0);
                    first_sect = false;
                }
                fprintf(objectFile, "\n\n\n");

                char* label = &LABEL[0];
                fprintf(objectFile, "H%-6s%06X%06X\n", label, 0, base->length);
            }

            if (words > 2){
                OPERAND = args[2];
            }

            extended = false;

            if (OPCODE[0] == '+'){
                extended = true;
                OPCODE=OPCODE.substr(1);
            }

            if (op_code *info = search_optab(OPCODE)){
                map <char,int> bits;
                bits['n']=0;
                bits['i']=0;
                bits['b']=0;
                bits['e']=0;
                bits['p']=0;
                bits['x']=0;
                int operand_value = 0;

                if (info->format == 2){
                    strcat(objcode, info->op_addr.c_str());
                    char* operand = &OPERAND[0];
                    int size = readLine(operand, 0, args, ",");
                    strcat(objcode, register_list[args[0]].c_str());
                    if (size == 2){
                        strcat(objcode, register_list[args[1]].c_str());
                    }else{
                        strcat(objcode, "0");
                    }
                }else if (info->format == 3 && !extended){
                    if (len > 1 && OPERAND[OPERAND.length() - 1] == 'X' && OPERAND[OPERAND.length() - 2] == ','){
                        bits['x'] = 1;
                        OPERAND[OPERAND.length() - 2] = '\0';
                    }

                    if (words > 2){
                        if (OPERAND[0] == '#'){
                            bits['n'] = 0;
                            OPERAND = OPERAND.substr(1);
                        }else{
                            bits['n'] = 1;
                        }

                        if (OPERAND[0] == '@'){
                            bits['i'] = 0;
                            OPERAND = OPERAND.substr(1);
                        }else{
                            bits['i'] = 1;
                        }

                        if (!isdigit(OPERAND[0])){
                            string sym = base->search_symtab(OPERAND);
                            operand_value = (int)strtol(sym.c_str(), NULL, 16) - hexToInt(addr) - 3;
                            if (operand_value < 0){
                                operand_value += 1 << 12;
                            }
                            bits['p'] = 1;
                        }else{
                            bits['p'] = 0;
                            char * operand = &OPERAND[0]; 
                            operand_value = (int)strtol(operand, NULL, 10);
                        }
                    }else{
                        bits['n'] = 1;
                        bits['i'] = 1;
                    }

                    int num_objcode = hexToInt(info->op_addr.c_str()) * pow(16, 4);
                    num_objcode |= operand_value;
                    num_objcode |= ((bits['n'] << 17) + (bits['i'] << 16) + (bits['x'] << 15) + (bits['b'] << 14) + (bits['p'] << 13) + (bits['e'] << 12));
                    sprintf(objcode, "%06X", num_objcode);
                }else if(info->format == 3 && extended){
                    if (words > 2){
                        if (OPERAND.length() > 1 && OPERAND[OPERAND.length() - 1] == 'X' && OPERAND[OPERAND.length() - 2] == ','){
                            bits['x'] = 1;
                            OPERAND[OPERAND.length() - 2] = '\0';
                        }

                        if (OPERAND[0] == '#'){
                            bits['n'] = 0;
                            OPERAND = OPERAND.substr(1);
                        }else{
                            bits['n'] = 1;
                        }

                        if (OPERAND[0] == '@'){
                            bits['i'] = 0;
                            OPERAND = OPERAND.substr(1);
                        }else{
                            bits['i'] = 1;
                        }

                        bits['e'] = 1;

                        modification_records.push({hexToInt(addr) + 1, 5, true, OPERAND});
                    }
                    int num_objcode = hexToInt(info->op_addr.c_str()) * pow(16, 6);
                    num_objcode |= ((bits['n'] << 17) + (bits['i'] << 16) + (bits['x'] << 15) + (bits['b'] << 14) + (bits['p'] << 13) + (bits['e'] << 12)) << 8;
                    sprintf(objcode, "%08X", num_objcode);
                }
            }
            else if (OPCODE=="BYTE"){
                if (OPERAND[0] == 'C'){
                    int c;
                    for (int i = 2; i < OPERAND.length() - 1; i++){
                        int c = OPERAND[i];
                        char temp[2];
                        sprintf(temp, "%0X", c);
                        strcat(objcode, temp);
                    }
                }else if (OPERAND[0] == 'X'){
                    char* operand = &OPERAND[0];
                    strcat(objcode, operand+2);
                    objcode[strlen(objcode) - 1] = '\0';
                }
            }else if (OPCODE=="WORD"){
                char* operand = &OPERAND[0]; 
                sprintf(objcode, "%06X", (int)strtol(operand, NULL, 10));
                int size = readLine(operand, 0, args, "+");
                if (size == 2){
                    modification_records.push({hexToInt(addr), 6, true, (string)args[0]});
                    modification_records.push({hexToInt(addr), 6, true, (string)args[1]});
                }else{
                    char* operand = &OPERAND[0];
                    size = readLine(operand, 0, args, "-");
                    if (size == 2){
                        modification_records.push({hexToInt(addr), 6, true, (string)args[0]});
                        modification_records.push({hexToInt(addr), 6, false, (string)args[1]});
                    }else{
                        modification_records.push({hexToInt(addr), 6, true, (string)args[0]});
                    }
                }
            }else if (OPCODE=="EXTREF"){
                fprintf(objectFile, "R");
                char* operand = &OPERAND[0];
                int size = readLine(operand, 0, args, ",");
                for (int i = 0; i < size; i++){
                    fprintf(objectFile, "%-6s", args[i]);
                }
                fprintf(objectFile, "\n");
            }else if (OPCODE=="EXTDEF"){
                fprintf(objectFile, "D");
                char* operand = &OPERAND[0];
                int size = readLine(operand, 0, args, ",");
                for (int i = 0; i < size; i++){
                    fprintf(objectFile, "%-6s%06X", args[i], hexToInt(base->search_symtab(args[i]).c_str()));
                }
                fprintf(objectFile, "\n");
            }else if (OPCODE=="LTORG"){
            }else if (OPCODE=="EQU"){
                char* operand = &OPERAND[0];
                int size = readLine(operand, 0, args, "+");
                if (size == 2){
                    string sym = base->search_symtab(args[0]);
                    int val = hexToInt(sym.c_str());
                    sym = base->search_symtab(args[1]);
                    val += hexToInt(sym.c_str());
                    sprintf(addr, "%04X", val);
                }else{
                    char* operand = &OPERAND[0];
                    size = readLine(operand, 0, args, "-");
                    if (size == 2){
                        string sym = base->search_symtab(args[0]);
                        int val = hexToInt(sym.c_str());
                        sym = base->search_symtab(args[1]);
                        val -= hexToInt(sym.c_str());
                        sprintf(addr, "%04X", val);
                    }else if (OPERAND!="*"){
                        string sym = base->search_symtab(args[0]);
                        int val = hexToInt(sym.c_str());
                        sprintf(addr, "%04X", val);
                    }
                }
            }

            if (LABEL[0] == '*'){
                char* label = &LABEL[0];
                readLine(label, 0, args, "=");
                OPERAND = args[1];
                if (OPERAND[0] == 'C'){
                    int c;
                    for (int i = 2; i < OPERAND.length() - 1; i++){
                        int c = OPERAND[i];
                        char temp[2];
                        sprintf(temp, "%0X", c);
                        strcat(objcode, temp);
                    }
                }
                else if (OPERAND[0] == 'X'){
                    char* operand = &OPERAND[0];
                    strcat(objcode, operand + 2);
                    objcode[strlen(objcode) - 1] = '\0';
                }
            }

            if (strlen(record) + strlen(objcode) > 60 || OPCODE=="RESW" || OPCODE=="RESB"){
                if (strlen(record) > 0){
                    fprintf(objectFile, "T%06X%02X%s\n", hexToInt(firstaddr),(int)strlen(record) / 2, record);
                }
                stringCopy(record, "");
            }

            if (strlen(record) == 0){
                stringCopy(firstaddr, addr);
            }
            strcat(record, objcode);

            line[strlen(line) - 1] = '\0';
            fprintf(listFile, "%s%-26s\t%s\n", addr, line, objcode);
        }else{
            fprintf(listFile, "\t%s", line);
        }

        if (fscanf(intermediateFile, "%[^\t]s", addr) == -1){
            break;
        }

        getline(&line, &len, intermediateFile);
        stringCopy(temp, line);
        words = readLine(temp, 0, args," ");
        LABEL = args[0];
        OPCODE = args[1];

        while (LABEL[0] == ' ' || LABEL[0] == '\t'){
            LABEL=LABEL.substr(1);
        }
    }

    if (strlen(record) > 1){
        fprintf(objectFile, "T%06X%02X%s\n", hexToInt(firstaddr),(int)strlen(record) / 2, record);
    }

    base = symtab_list[PROGNAME];

    while (!modification_records.empty()){
        modrec rec = modification_records.front();
        modification_records.pop();
        fprintf(objectFile, "M%06X%02X%s%s\n", rec.addr, rec.length, rec.sign ? "+" : "-", rec.symbol.c_str());
    }

    fprintf(objectFile, "E");

    if (first_sect){
        fprintf(objectFile, "%06X", 0);
        first_sect = false;
    }

    fprintf(objectFile, "\n\n\n");

    cout<<"PASS 2 DONE"<<endl;
}

int main(){

    load_OPTAB();

    FILE *st = fopen(symtabFile, "w");
    fclose(st);

    progamFile = fopen("program.txt", "r");
    intermediateFile = fopen("intermediate.txt", "w");
    objectFile = fopen("objectCodeFile.txt", "w");
    listFile = fopen("codeListingFile.txt", "w");

    pass1();

    fclose(intermediateFile);

    intermediateFile = fopen("intermediate.txt", "r");

    pass2();

    fclose(progamFile);
    fclose(intermediateFile);
    fclose(objectFile);

    return 0;
}