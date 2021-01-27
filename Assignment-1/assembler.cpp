#include <bits/stdc++.h>

using namespace std;

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

bool readline(string* LABEL,string* OPCODE,string* OPERAND){
    string line;
    getline(cin,line);
    *LABEL = "";
    *OPCODE = "";
    *OPERAND = "";
    int pointer = 0;
    if(line[0]=='.'){
        return false;
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
        if(line[pointer]==' '){
            break;
        }else{
            *OPERAND += line[pointer];
        }
    }
    return true;
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

void pass1(string LABEL,string OPCODE,string OPERAND){
    readline(&LABEL,&OPCODE,&OPERAND);
    string LOCCTR ;
    if(OPCODE == "START"){
        cout<<"YESS"<<endl;
    }else{
        LOCCTR = "000000";
    }
}

int main(){
    string LABEL, OPCODE, OPERAND;
    pass1(LABEL,OPCODE,OPERAND);
    return 0;
}