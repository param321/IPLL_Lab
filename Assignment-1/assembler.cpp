#include <bits/stdc++.h>

using namespace std;

const unordered_map<string,string> SYMTAB = {
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

void readline(string* LABEL,string* OPCODE,string* OPERAND){
    string line;
    getline(cin,line);
    *LABEL = "";
    *OPCODE = "";
    *OPERAND = "";
    int pointer = 0;
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
        if(line[pointer]==' '){
            break;
        }else{
            *OPERAND += line[pointer];
        }
    }
}

int main(){
    string LABEL, OPCODE, OPERAND;
    readline(&LABEL,&OPCODE,&OPERAND);
    return 0;
}