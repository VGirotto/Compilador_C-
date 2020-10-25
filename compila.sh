
flex scanner.l
bison -d -v sint.y

gcc -c *.c
gcc -o exec *.o -ly -lfl
./exec tests/sort.txt

rm analyze.o lex.yy.o main.o sint.output sint.tab.o symtab.o util.o cgen.o code.o assembly.o binario.o
rm lex.yy.c exec
