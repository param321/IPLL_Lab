-> inp.pas is attached and is input to our source code (180101055_lex.l and 180101055_yacc.y)
-> To run
Run following commands in sequence in the directory of source code files
1.) "yacc -d 180101055_yacc.y"
2.) "lex 180101055_lex.l"
3.) "gcc -w y.tab.c lex.yy.c"
4.) "./a.out < inp.pas"