/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    UNEG = 258,
    UPOS = 259,
    tPROGRAM = 260,
    tVAR = 261,
    tBEGIN = 262,
    tEND = 263,
    tEND_ = 264,
    tINTEGER = 265,
    tREAL = 266,
    tFOR = 267,
    tREAD = 268,
    tWRITE = 269,
    tTO = 270,
    tDO = 271,
    tSEMICOLON = 272,
    tCOLON = 273,
    tCOMMA = 274,
    tASSIGN = 275,
    tADD = 276,
    tSUBTRACT = 277,
    tMULT = 278,
    tDIV = 279,
    tBRACKET_OPEN = 280,
    tBRACKET_CLOSE = 281,
    tID = 282,
    tINT = 283,
    tReal = 284
  };
#endif
/* Tokens.  */
#define UNEG 258
#define UPOS 259
#define tPROGRAM 260
#define tVAR 261
#define tBEGIN 262
#define tEND 263
#define tEND_ 264
#define tINTEGER 265
#define tREAL 266
#define tFOR 267
#define tREAD 268
#define tWRITE 269
#define tTO 270
#define tDO 271
#define tSEMICOLON 272
#define tCOLON 273
#define tCOMMA 274
#define tASSIGN 275
#define tADD 276
#define tSUBTRACT 277
#define tMULT 278
#define tDIV 279
#define tBRACKET_OPEN 280
#define tBRACKET_CLOSE 281
#define tID 282
#define tINT 283
#define tReal 284

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 34 "180101055_yacc.y"

	char* str;
	int num;
	char id;
	char arr[25];

#line 122 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
