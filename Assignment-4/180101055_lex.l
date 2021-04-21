

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
