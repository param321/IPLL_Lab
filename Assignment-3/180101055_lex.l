/*
	Param Aryan Singh, 180101055
	Assignment-3  
	To run the lexical analyser, type the following the cmd:
		flex 180101055_lex.l 
		g++ lex.yy.c or g++ 180101055_lex.yy.c (same as 180101055_lex.yy.c)
		./a.out
     output.txt is generated on running lexical analyser
*/

%{ 

//Important header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <math.h>
#include <string>
#include <map>
#include <stdbool.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iomanip>

using namespace std;

// Token definitions (codes)
#define tProgram 1
#define tVar 2
#define tBegin 3
#define tEnd 4
#define tEndDot 5
#define tInteger 6
#define tFor 7
#define tRead 8
#define tWrite 9
#define tTo 10
#define tDo 11
#define tSemicolon 12
#define tColon 13
#define tComma 14
#define tAssignment 15
#define tPlus 16
#define tMinus 17
#define tMultiply 18
#define tDivision 19
#define tOpenBracket 20
#define tCloseBracket 21
#define tID 22
#define tInt 23

bool install_num(char *str);
bool install_id(char *str);
//auxillary functions to add number or id in hash table

int line_count=0; 
// to count number of lines

char* id;
int integer;
//to store yytext in these variables

string yytextString;
// to store char * as string type

unordered_map<string,int> hashTable; 
// hash table to store symbol table
// int values when int->1 and when id->0

%}

%option noyywrap

%%

PROGRAM 			{return tProgram;}
VAR				    {return tVar;}
BEGIN  			    {return tBegin;}
END 				{return tEnd;}
"END."          	{return tEndDot;}
INTEGER         	{return tInteger;}
FOR 				{return tFor;}	
READ 				{return tRead;}
WRITE 			    {return tWrite;}
TO  				{return tTo;}
DO 				    {return tDo;}
";" 				{return tSemicolon;}
":="  				{return tAssignment;}
":" 				{return tColon;}
"," 				{return tComma;}
"+" 				{return tPlus;}
"-" 				{return tMinus;}
"*" 				{return tMultiply;}
DIV 				{return tDivision;}
"(" 				{return tOpenBracket;}
")"					{return tCloseBracket;}
"\n"				{line_count++;}
[A-Z]([A-Z0-9])* 	{
						install_id(yytext);
                        id=yytext;
						return tID;
					}
[0-9]+				{
						install_num(yytext);
                        integer=atoi(yytext);
						return tInt;
					}
"\t"                {}
.					{}

%%

bool install_id(char *str){
    yytextString=yytext;
    if(hashTable.find(yytextString)==hashTable.end()){ //not present in hash table
        hashTable[yytextString]=0;
        return true;
    }
    return false; //already present in hash table
}

bool install_num(char *str){
    yytextString=yytext;
    if(hashTable.find(yytextString)==hashTable.end()){ //not present in hash table
        hashTable[yytextString]=1;
        return true;
    }
    return false; //already present in hash table
}

int main(){
	int s=0;
	yyin = fopen("input.txt","r"); // open the input file
    yyout = fopen("output.txt","w"); //open the output file
    if(!(yyin&&yyout)){
		return 0;
	}
	fprintf(yyout,"Line\t\tToken\t\tToken Specifier\n");
    int prevLineCount=-1;
    //read the input file 
	while(s=yylex()){ 
	//as per the matched token we print in the output file
        //when token code is id or int
        if(s==tInt){
            if(prevLineCount!=line_count){
                fprintf(yyout,"%d\t\t%d\t\t#%d\n",line_count+1,s,integer);
            } 
            else{
                fprintf(yyout,"\t\t%d\t\t#%d\n",s,integer);
            }
        }
        else if(s==tID){
            if(prevLineCount!=line_count){
                fprintf(yyout,"%d\t\t%d\t\t^%s\n",line_count+1,s,id);
            } 
            else{
                fprintf(yyout,"\t\t%d\t\t^%s\n",s,id);
            }
        }
        //when token is any other value except id or int
        else{
            if(prevLineCount!=line_count){
                fprintf(yyout,"%d\t\t%d\n",line_count+1,s);
            } 
            else{
                fprintf(yyout,"\t\t%d\n",s);
            }
        }
		prevLineCount=line_count;
	}
	return 0;
}
