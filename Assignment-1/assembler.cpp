#include <bits/stdc++.h>

using namespace std;

string PROG_LEN;

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

bool readline(string* LABEL,string* OPCODE,string* OPERAND,ifstream *fin){
    string line;
    getline(*fin,line);
    *LABEL = "";
    *OPCODE = "";
    *OPERAND = "";
    int pointer = 0;
    if(line[0]=='.'){
        *LABEL = line;
        return true;
    }
    if(line[0]!=' '){
        for(pointer = 0;pointer < line.length();pointer++){
            if(line[pointer]==' '){
                break;
            }else{
                *LABEL += line[pointer];
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
            *OPCODE += line[pointer];
        }
    }
    while(line[pointer]==' '){
        pointer++;
    }
    for(;pointer < line.length();pointer++){
        if(line[pointer]==' '||line[pointer]==','){
            break;
        }else{
            *OPERAND += line[pointer];
        }
    }
    return false;
}

void writeline(string LABEL,string OPCODE,string OPERAND,ofstream* fout){
    *fout<<LABEL<<"  "<<OPCODE<<"  "<<OPERAND<<endl;
}

char till15toHex(int n){
    char c;
    if(n<=9){
        c = '0'+n;
    }else{
        c = 'A'+(n-10);
    }
    return c;
}

int till15toInt(char c){
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
        s = till15toHex(n%16) + s;
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
        s = till15toHex(n%16) + s;
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
        ans+= (till15toInt(s[i])*j);
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

void incrLOCCTR(string* LOCCTR,string OPCODE,string OPERAND){
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

string assembleObjectCode(string OPERAND_ADDR,string OPCODE){
    string ObjectCode = OPTAB.at(OPCODE);
    ObjectCode += OPERAND_ADDR.substr(2);
    return ObjectCode;
}

void pass1(string LABEL,string OPCODE,string OPERAND){

    ifstream fin; 
    fin.open("source.txt");

    ofstream fout;
    fout.open("intermediate.txt");

    bool ifcomment = false;
    string LOCCTR ;
    string START_ADDR = intToHex(0);
    ifcomment = readline(&LABEL,&OPCODE,&OPERAND,&fin);
    
    if(OPCODE == "START"){
        START_ADDR = (intToHex(hexToInt(OPERAND)));
        LOCCTR = (intToHex(hexToInt(OPERAND)));
        writeline(LABEL,OPCODE,OPERAND,&fout);
        ifcomment = readline(&LABEL,&OPCODE,&OPERAND,&fin);
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
            incrLOCCTR(&LOCCTR,OPCODE,OPERAND);
        }
        writeline(LABEL,OPCODE,OPERAND,&fout);
        ifcomment = readline(&LABEL,&OPCODE,&OPERAND,&fin);
    }

    writeline(LABEL,OPCODE,OPERAND,&fout);
    PROG_LEN = intToHex(hexToInt(LOCCTR)-hexToInt(START_ADDR));
    fout.close();
    fin.close();
}

void pass2(string LABEL,string OPCODE,string OPERAND){

    ifstream fin; 
    fin.open("intermediate.txt");

    ofstream fout;
    fout.open("object.txt");

    bool ifcomment = false;

    string LOCCTR = intToHex(0);
    string START_ADDR = intToHex(0);

    ifcomment = readline(&LABEL,&OPCODE,&OPERAND,&fin);

    string prevLABEL = LABEL;
    string prevOPERAND = OPERAND;

    if(OPCODE == "START"){
        START_ADDR = (intToHex(hexToInt(OPERAND)));
        LOCCTR = (intToHex(hexToInt(OPERAND)));
        ifcomment = readline(&LABEL,&OPCODE,&OPERAND,&fin);
    }

    string HEADER = "H";
    HEADER+=prevLABEL;
    HEADER+=" ";
    HEADER+=LOCCTR;
    HEADER+=PROG_LEN;
    writeline(HEADER,"","",&fout);

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
                            OPERAND_ADDR = intToHex((8*16*16*16) + hexToInt(OPERAND_ADDR));
                        }
                    }else{
                        OPERAND_ADDR = intToHex(0);
                        cout<<"ERROR:UNDEFINED SYMBOL"<<endl;
                    }
                }else{
                    OPERAND_ADDR = intToHex(0);
                }
                OBJECT_CODE = assembleObjectCode(OPERAND_ADDR,OPCODE);
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
            if(OBJECT_CODE.length()+TEXT.length()>69){
                string len = intToHexLen2((TEXT.length()-9)/2);
                TEXT[7]=len[0];
                TEXT[8]=len[1];
                writeline(TEXT,"","",&fout);
                TEXT="T";
                TEXT+=LOCCTR;
                TEXT+="  ";
            }
            TEXT+=OBJECT_CODE;
            incrLOCCTR(&LOCCTR,OPCODE,OPERAND);
        }
        ifcomment = readline(&LABEL,&OPCODE,&OPERAND,&fin);
    }

    string len = intToHexLen2((TEXT.length()-9)/2);
    TEXT[7]=len[0];
    TEXT[8]=len[1];
    writeline(TEXT,"","",&fout);

    string endRecord="E";
    endRecord+=START_ADDR;
    writeline(endRecord,"","",&fout);
    fout.close();
    fin.close();
}

int main(){
    string LABEL, OPCODE, OPERAND;
    pass1(LABEL,OPCODE,OPERAND);
    pass2(LABEL,OPCODE,OPERAND);
    return 0;
}