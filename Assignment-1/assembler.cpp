#include <bits/stdc++.h>

using namespace std;

string PROG_LEN;

const int TEXT_RECORD_LEN = 69;

const unordered_map<string,string> OPTAB = {
    {"LDA","00"},
    {"LDX","04"},
    {"LDL","08"},
    {"STA","0C"},
    {"STX","10"},
    {"STL","14"},
    {"LDCH","50"},
    {"STCH","54"},
    {"ADD","18"},
    {"SUB","1C"},
    {"MUL","20"},
    {"DIV","24"},
    {"COMP","28"},
    {"J","3C"},
    {"JLT","38"},
    {"JEQ","30"},
    {"JGT","34"},
    {"JSUB","48"},
    {"RSUB","4C"},
    {"TIX","2C"},
    {"TD","E0"},
    {"RD","D8"},
    {"WD","DC"}
};

unordered_map <string,string> SYMTAB;

string LABEL;
string OPCODE;
string OPERAND;

bool readInputLine(ifstream *fin){
    string line;
    getline(*fin,line);
    LABEL = "";
    OPCODE = "";
    OPERAND = "";
    int pointer = 0;

    if(line[0]=='.'){
        LABEL = line;
        return true;
    }

    if(line[0]!=' '){
        for(pointer = 0;pointer < line.length();pointer++){
            if(line[pointer]==' '){
                break;
            }else{
                LABEL += line[pointer];
            }
        }
    }

    while(line[pointer]==' '){
        pointer++;
    }

    for(;pointer < line.length();pointer++){
        if(line[pointer]==' '){
            break;
        }else{
            OPCODE += line[pointer];
        }
    }

    while(line[pointer]==' '){
        pointer++;
    }

    for(;pointer < line.length();pointer++){
        if(line[pointer]==' '||line[pointer]==','){
            break;
        }else{
            OPERAND += line[pointer];
        }
    }

    return false;
}

bool readIntermediateLine(string* LOCCTR,ifstream *fin){
    string line;
    getline(*fin,line);

    *LOCCTR="";
    LABEL = "";
    OPCODE = "";
    OPERAND = "";
    int pointer = 0;

    if(line[0]=='.'){
        LABEL = line;
        return true;
    }

    if(line[0]!=' '){
        for(pointer = 0;pointer < line.length();pointer++){
            if(line[pointer]==' '){
                break;
            }else{
                *LOCCTR += line[pointer];
            }
        }
    }

    pointer+=2;

    for(;pointer < line.length();pointer++){
        if(line[pointer]==' '){
            break;
        }else{
            LABEL += line[pointer];
        }
    }

    pointer+=2;

    for(;pointer < line.length();pointer++){
        if(line[pointer]==' '){
            break;
        }else{
            OPCODE += line[pointer];
        }
    }

    pointer+=2;

    for(;pointer < line.length();pointer++){
        if(line[pointer]==' '||line[pointer]==','){
            break;
        }else{
            OPERAND += line[pointer];
        }
    }

    return false;
}

void writeIntermediateLine(string LOCCTR, ofstream* fout){
    if(OPCODE!=""){
        *fout<<LOCCTR<<"  "<<LABEL<<"  "<<OPCODE<<"  "<<OPERAND<<endl;
    }else{
        *fout<<LOCCTR<<endl;
    }
}

void writeObjectLine(string line,ofstream* fout){
    *fout<<line<<endl;
}

char intToHexSingleDigit(int n){
    char c;
    if(n<=9){
        c = '0'+n;
    }else{
        c = 'A'+(n-10);
    }
    return c;
}

int hexToIntSingleChar(char c){
    int n;
    if((c<='9')&&(c>='0')){
        n = c-'0';
    }else{
        n = c -'A' + 10;
    }
    return n;
}

string intToHex(int n){
    string s = "";
    while(n!=0){
        s = intToHexSingleDigit(n%16) + s;
        n = n / 16;
    }
    while(s.length()<6){
        s = '0' + s;
    }
    return s;
}

string intToHexLen2(int n){
    string s = "";
    while(n!=0){
        s = intToHexSingleDigit(n%16) + s;
        n = n / 16;
    }
    while(s.length()<2){
        s = '0' + s;
    }
    return s;
}

int hexToInt(string s){
    reverse(s.begin(),s.end());
    int j=1;
    int ans=0;
    for(int i=0;i<s.length();i++){
        ans+= (hexToIntSingleChar(s[i])*j);
        j=j*16;
    }
    return ans;
}

int stringToInt(string s){
    reverse(s.begin(),s.end());
    int j=1;
    int ans=0;
    for(int i=0;i<s.length();i++){
        ans+= ((s[i]-'0')*j);
        j=j*10;
    }
    return ans;
}

int bytelength(string s){
    int len = 0;
    if(s[0]=='X'){
        len = (s.length()-3)/2;
    }else if(s[0]=='C'){
        len = s.length() -3;
    }
    return len;
}

void incrLOCCTR(string* LOCCTR){
    if(OPTAB.find(OPCODE)!=OPTAB.end()){
        *LOCCTR = intToHex(3 + hexToInt(*LOCCTR));
    }else if(OPCODE == "WORD"){
        *LOCCTR = intToHex(3 + hexToInt(*LOCCTR));
    }else if(OPCODE == "RESW"){
        *LOCCTR = intToHex((3*stringToInt(OPERAND)) + hexToInt(*LOCCTR));
    }else if(OPCODE == "RESB"){
        *LOCCTR = intToHex(stringToInt(OPERAND) + hexToInt(*LOCCTR));
    }else if(OPCODE == "BYTE"){
        *LOCCTR = intToHex(bytelength(OPERAND) + hexToInt(*LOCCTR));
    }else{
        cout<<"ERROR:INVALID OPCODE"<<endl;
    }
}

string assembleObjectCode(string OPERAND_ADDR){
    string ObjectCode = OPTAB.at(OPCODE);
    ObjectCode += OPERAND_ADDR.substr(2);
    return ObjectCode;
}

void pass1(){

    ifstream fin; 
    fin.open("source.txt");

    ofstream fout;
    fout.open("intermediate.txt");

    bool ifcomment = false;
    string LOCCTR =intToHex(0);
    string START_ADDR = intToHex(0);
    ifcomment = readInputLine(&fin);
    
    if(OPCODE == "START"){
        START_ADDR = (intToHex(hexToInt(OPERAND)));
        LOCCTR = (intToHex(hexToInt(OPERAND)));
        writeIntermediateLine(LOCCTR,&fout);
        ifcomment = readInputLine(&fin);
    }else{
        LOCCTR = intToHex(0);
    }

    while(OPCODE != "END"){
        if(ifcomment == false){
            if(LABEL!=""){
                if(SYMTAB.find(LABEL)!=SYMTAB.end()){
                    cout<<"ERROR:DUPLICATE SYMBOL"<<endl;
                }else{
                    SYMTAB[LABEL]=LOCCTR;
                }
            }
            writeIntermediateLine(LOCCTR,&fout);
            incrLOCCTR(&LOCCTR);
        }else{
            string comment = LABEL;
            LABEL="";
            writeIntermediateLine(comment,&fout);
        }
        ifcomment = readInputLine(&fin);
    }

    writeIntermediateLine(LOCCTR,&fout);

    PROG_LEN = intToHex(hexToInt(LOCCTR)-hexToInt(START_ADDR));

    fout.close();
    fin.close();
}

void pass2(){

    ifstream fin; 
    fin.open("intermediate.txt");

    ofstream fout;
    fout.open("object.txt");

    bool ifcomment = false;

    string LOCCTR = intToHex(0);
    string START_ADDR = intToHex(0);

    ifcomment = readIntermediateLine(&LOCCTR,&fin);

    string prevLABEL = LABEL;
    string prevOPERAND = OPERAND;

    if(OPCODE == "START"){
        START_ADDR = (intToHex(hexToInt(OPERAND)));
        ifcomment = readIntermediateLine(&LOCCTR,&fin);
    }

    string HEADER = "H";
    HEADER+=prevLABEL;
    for(int i=0;i<6-prevLABEL.length();i++){
        HEADER+=" ";
    }
    HEADER+=LOCCTR;
    HEADER+=PROG_LEN;
    writeObjectLine(HEADER,&fout);

    string TEXT = "T";
    TEXT+=LOCCTR;
    TEXT+="  ";

    string OPERAND_ADDR;
    string OBJECT_CODE;

    while(OPCODE != "END"){
        if(ifcomment == false){
            if(OPTAB.find(OPCODE)!=OPTAB.end()){
                if(OPERAND!=""){
                    if(SYMTAB.find(OPERAND)!=SYMTAB.end()){
                        OPERAND_ADDR = SYMTAB[OPERAND];
                        if(OPCODE == "LDCH"|| OPCODE=="STCH" ){
                            OPERAND_ADDR = intToHex(hexToInt("008000") + hexToInt(OPERAND_ADDR));
                        }
                    }else{
                        OPERAND_ADDR = intToHex(0);
                        cout<<"ERROR:UNDEFINED SYMBOL"<<endl;
                    }
                }else{
                    OPERAND_ADDR = intToHex(0);
                }
                OBJECT_CODE = assembleObjectCode(OPERAND_ADDR);
            }else if(OPCODE == "BYTE" || OPCODE == "WORD"){
                if(OPCODE == "WORD"){
                    OBJECT_CODE = intToHex(stringToInt(OPERAND));
                }else{
                    if(OPERAND[0]=='C'){
                        OBJECT_CODE="";
                        for(int i=2;i<OPERAND.length()-1;i++){
                            OBJECT_CODE+=intToHexLen2(OPERAND[i]);
                        }
                    }else{
                        OBJECT_CODE="";
                        for(int i=2;i<OPERAND.length()-1;i++){
                            OBJECT_CODE+=OPERAND[i];
                        }
                    }
                }
            }

            if(OBJECT_CODE.length()+TEXT.length()>TEXT_RECORD_LEN||((OPCODE=="RESW"||OPCODE=="RESB")&&TEXT.length()>9)){
                string len = intToHexLen2((TEXT.length()-9)/2);
                TEXT[7]=len[0];
                TEXT[8]=len[1];
                writeObjectLine(TEXT,&fout);
                TEXT="T";
                TEXT+=LOCCTR;
                TEXT+="  ";
            }

            if(TEXT.length()==9){
                TEXT="T";
                TEXT+=LOCCTR;
                TEXT+="  ";
            }

            if(OPCODE!="RESW"&&OPCODE!="RESB"){
                TEXT+=OBJECT_CODE;
            }
        }
        ifcomment = readIntermediateLine(&LOCCTR,&fin);
    }

    string len = intToHexLen2((TEXT.length()-9)/2);
    TEXT[7]=len[0];
    TEXT[8]=len[1];
    writeObjectLine(TEXT,&fout);

    string endRecord="E";
    endRecord+=START_ADDR;
    writeObjectLine(endRecord,&fout);

    fout.close();
    fin.close();
}

int main(){
    pass1();
    pass2();
    return 0;
}