/*
	Param Aryan Singh, 180101055
	Assignment-4  
	To run-
	Run following commands in sequence in the directory of source code files
	1.) "yacc -d 180101055_yacc.y"
	2.) "lex 180101055_lex.l"
	3.) "gcc -w y.tab.c lex.yy.c"
	4.) "./a.out < inp.pas"
*/

%{
    #include<string.h>
	#include <stdbool.h>
	#include<stdio.h>
	#include "y.tab.h"
%}

underScore = [_]
allAlphabet = [a-zA-Z]
allDigits = [0-9]
real = {allDigits}+(\.{allDigits}+)?(E[+-]?{allDigits}+)?
allNumbers = {allDigits}+
id = {allAlphabet}({underScore|{allAlphabet}|{allDigits}})*

%%
"PROGRAM"		{return tPROGRAM;}
"VAR"			{return tVAR;}	
"BEGIN"			{return tBEGIN;}
"INTEGER"		{return tINTEGER;}
"REAL"			{return tREAL;}
"FOR"			{return tFOR;}
"READ"			{return tREAD;}
"WRITE"			{return tWRITE;}
"TO"			{return tTO;}
"DO"			{return tDO;}
"END"			{return tEND;}
"END."			{return tEND_;}	
":="			{return tASSIGN;}
"+"			{return tADD;}
"-"			{return tSUBTRACT;}
"*"			{return tMULT;}
"DIV"			{return tDIV;}
"("			{return tBRACKET_OPEN;}
")"			{return tBRACKET_CLOSE;}
";"			{return tSEMICOLON;}
":"			{return tCOLON;}
","			{return tCOMMA;}
\n 			{yylineno++;}

[a-zA-Z]([a-zA-Z0-9_])*	{
        yylval.str = strdup(yytext); return tID;
    }

[0-9]+			{return tINT;}
[0-9]+(\.[0-9]+)?(E[+-]?[0-9]+)? { return tReal;}	
[ \t\n]			;	
.			printf("Invalid Character \n");
%%

int yywrap(){
	return 1;
}
