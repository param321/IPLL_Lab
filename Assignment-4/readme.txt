yacc -d 180101055_yacc.y
lex 180101055_lex.l
gcc -w y.tab.c lex.yy.c
./a.out < inp.pas
rm y.tab.c lex.yy.c y.tab.h a.out