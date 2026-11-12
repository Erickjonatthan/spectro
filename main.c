/*  Arquivo temporario */

#include <stdio.h>

extern int yyparse(void);
extern FILE *yyin;

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Erro ao abrir arquivo");
            return 1;
        }
    }
    if (yyparse() == 0) {
        printf("Análise léxica e sintática concluída com sucesso!\n");
    }
    return 0;
}