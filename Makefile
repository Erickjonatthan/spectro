# Ordem: Bison gera o parser (e o .h com os tokens) -> Flex gera o lexer -> gcc compila tudo

mini: sintatico.y lexico.l arvore.c interpretador.c main.c
	bison -d sintatico.y
	flex lexico.l
	gcc -o mini sintatico.tab.c lex.yy.c arvore.c interpretador.c main.c

clean:
	rm -f mini sintatico.tab.c sintatico.tab.h lex.yy.c
