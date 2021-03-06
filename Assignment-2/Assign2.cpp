/*
ENVIRONMENT: LINUX g++ 
gcc version : 9.3.0
Command To compile the source code : g++ Assign2.cpp
Command to execute : ./a.out
*/

#include <bits/stdc++.h>

using namespace std;

const int MAXS = 1024;
const int MAXW = 10;

const int ILEN = 1 ;// Instruction length
const int WLEN = 1 ;// Word length

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
char PROGNAME[MAXS]; // store program length
int STARTADDR;       // store program length

struct op_code{
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

op_code* search_optab(char *OPCODE){
	string opcode = OPCODE;
	if (OPTAB.find(opcode) != OPTAB.end()){
		return &OPTAB[opcode];
	}
	return NULL;
}


struct symbol_info{
    string addr;
};

class symtab{
public:
    unordered_map<string, symbol_info> table;
    set<string> extref;
    set<string> extdef;
    int length;

    symtab(){}

    symbol_info* search_symtab(char *LABEL){
        string label = LABEL;
        table.find("RLOOP");
        if (table.find(label) != table.end()){
            return &table[label];
        }
        return NULL;
    }
    
    void insert_symtab(const char *LABEL, int LOCCTR){
        FILE *filesymtab = fopen(symtabFile, "a");
        string label = LABEL;
        char addr[MAXW];
        sprintf(addr, "%0X", LOCCTR);
        table[label] = {addr};
        fprintf(filesymtab, "%s\t%s\n", addr, LABEL);
        fclose(filesymtab);
    }
    
    void insert_extdef(char *LABEL){
        extdef.insert((string)LABEL);
    }

    void insert_extref(char *LABEL){
	    extref.insert((string)LABEL);
    }

    void print_symtab(){
        for (auto i :table){
            cout << i.first <<" "<< i.second.addr << endl;
        }
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

int readLine(char *str, int start, char *words[], const char *delimiter=" "){
	str = strtok(str, "\n");

	int size = start; // Store number of words
	char *ptr = strtok(str, delimiter);

	while (ptr != NULL){
		words[size++] = ptr;
		ptr = strtok(NULL, delimiter);
	}

	return size;
}

int print_literals(int LOCCTR, FILE *outputFile, symtab *base){
	while (literal_pool.size()){
		string lit = *literal_pool.begin();
		fprintf(outputFile, "%04X\t*\t%s\n", LOCCTR, lit.c_str());
		base->insert_symtab(lit.c_str(), LOCCTR);
		if (lit[1] == 'C'){
			LOCCTR += lit.length() - 4;
		}else{
			LOCCTR += (lit.length() - 4 + 1) / 2;
		}
		literal_pool.erase(literal_pool.begin());
	}
	return LOCCTR;
}

FILE *progamFile, *intmediateFile, *objectFile, *listFile;

void pass1(){

    fseek(progamFile, 0, SEEK_SET);

    //clear symbol table
    FILE *st = fopen(symtabFile, "w");
    fclose(st);

    char *line = NULL, temp[MAXS];
    size_t len = 0;
    char *args[MAXW];
    int words;
    char *OPCODE, *LABEL, *OPERAND;

    // store addresses
    int LOCCTR;

    // read first line
    getline(&line, &len, progamFile);
    strcpy(temp, line);

    if(line[0] == ' '){
        words = readLine(temp, 1, args);
        LABEL = NULL;
        OPCODE = args[1];
    }else{
        words = readLine(temp, 0, args);
        LABEL = args[0];
        OPCODE = args[1];
    }

    if(strcmp(args[1], "START") == 0){
        OPERAND = args[2];
        STARTADDR = strtol(OPERAND, NULL, 16);
        strcpy(PROGNAME, LABEL);
        LOCCTR = STARTADDR;

        // write line to intermediate file
        fprintf(intmediateFile, "%04X\t%s", LOCCTR, line);

        // read next input line
        getline(&line, &len, progamFile);
        strcpy(temp, line);
        if (line[0] == ' '){
            words = readLine(temp, 1, args);
            LABEL = NULL;
            OPCODE = args[1];
        }else{
            words = readLine(temp, 0, args);
            LABEL = args[0];
            OPCODE = args[1];
        }
    }else{
        LOCCTR = 0;
    }

    symtab *base = new symtab;
    symtab_list[(string)PROGNAME] = base;
    while (strcmp(OPCODE, "END") != 0){
        if (line[0] != '.'){ // check if not a comment
            if (strcmp(OPCODE, "CSECT") == 0){
                // DO THIS
                LOCCTR = print_literals(LOCCTR, intmediateFile, base);
                base->length = LOCCTR;
                base = new symtab;
                symtab_list[(string)LABEL] = base;
                LOCCTR = 0;
            }else if (LABEL){ // check if symbol in label
                if (base->search_symtab(LABEL)){
                    printf("Error: Duplicate Symbol. %s\n", LABEL);
                    exit(0);
                }else{
                    base->insert_symtab(LABEL, LOCCTR);
                }
            }

            bool extended = false;

            if (OPCODE[0] == '+'){
                extended = true;
                OPCODE++;
            }

            if (words > 2){
                OPERAND = args[2];
                if (OPERAND[0] == '='){
                    literal_pool.insert((string)OPERAND);
                }
            }

            // write line to intermediate file
            if (strcmp(OPCODE, "EXTDEF") == 0 || strcmp(OPCODE, "EXTREF") == 0 || strcmp(OPCODE, "LTORG") == 0){
                fprintf(intmediateFile, "    \t%s", line);
            }else{
                fprintf(intmediateFile, "%04X\t%s", LOCCTR, line);
            }

            if (op_code *info = search_optab(OPCODE)){
                if (extended){
                    LOCCTR = LOCCTR + 4 * ILEN;
                }else{
                    LOCCTR = LOCCTR + info->format * ILEN;
                }
            }else if (strcmp(OPCODE, "WORD") == 0){
                LOCCTR = LOCCTR + 3;
            }else if (strcmp(OPCODE, "RESW") == 0){
                LOCCTR = LOCCTR + (3 * strtol(OPERAND, NULL, 10));
            }else if (strcmp(OPCODE, "RESB") == 0){
                LOCCTR = LOCCTR + strtol(OPERAND, NULL, 10);
            }else if (strcmp(OPCODE, "BYTE") == 0){
                if (OPERAND[0] == 'C'){
                    LOCCTR = LOCCTR + (strlen(OPERAND)-3);
                }else if (OPERAND[0] == 'X'){
                    LOCCTR = LOCCTR + ((strlen(OPERAND)-3+1)/2);
                }
            }else if (strcmp(OPCODE, "EXTREF") == 0){
                // DO THIS
                int size = readLine(OPERAND, 0, args, ",");
                for (int i = 0; i < size; i++){
                    base->insert_extref(args[i]);
                }
            }else if (strcmp(OPCODE, "EXTDEF") == 0){
                // DO THIS
                OPERAND = args[2];
                int size = readLine(OPERAND, 0, args, ",");
                for (int i = 0; i < size; i++){
                    base->insert_extdef(args[i]);
                }
            }else if (strcmp(OPCODE, "LTORG") == 0){
                // DO THIS
                LOCCTR = print_literals(LOCCTR, intmediateFile, base);
            }else if (strcmp(OPCODE, "EQU") == 0){
                // DO THIS
            }else if (strcmp(OPCODE, "CSECT") == 0){
                // DO THIS
            }else{
                printf("Error: Invalid operation code. \n");
                exit(0);
            }
        }else{
            fprintf(intmediateFile, "    \t%s", line);
        }

        // read next input line
        getline(&line, &len, progamFile);
        strcpy(temp, line);
        if (line[0] == ' '){
            words = readLine(temp, 1, args);
            LABEL = NULL;
            OPCODE = args[1];
        }else{
            words = readLine(temp, 0, args);
            LABEL = args[0];
            OPCODE = args[1];
        }
    }

    fprintf(intmediateFile, "    \t%s\n", line);

    LOCCTR = print_literals(LOCCTR, intmediateFile, base);
    base->length = LOCCTR;

    // store program length
    PROGLEN = LOCCTR - STARTADDR;

    cout<<"PASS 1 DONE"<<endl;
}

int main(){

    load_OPTAB();

    progamFile = fopen("program.txt", "r");
    intmediateFile = fopen("intermediate.txt", "w");
    objectFile = fopen("objectCodeFile.txt", "w");
    listFile = fopen("codeListingFile.txt", "w");

    pass1();

    fclose(intmediateFile);

    intmediateFile = fopen("intermediate.txt", "r");

    //pass2(intm, out, list);

    fclose(progamFile);
    fclose(intmediateFile);
    fclose(objectFile);

    return 0;
}