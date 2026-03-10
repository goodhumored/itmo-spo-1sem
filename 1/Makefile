parser: parser.tab.c lex.yy.c main.c ast.c
	gcc -Wall -o parser parser.tab.c lex.yy.c ast.c main.c

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

clean:
	rm -f parser.tab.* lex.yy.c parser
