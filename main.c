/*
 * main.c — Ponto de entrada do interpretador Spectro.xum
 *
 * Fluxo:
 *   código-fonte --(Flex)--> tokens --(Bison)--> árvore AST --(interpretador)--> execução
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"

extern FILE *yyin;
extern No *raiz;
int yyparse(void);

int main(int argc, char *argv[]) {
    int mostrar_arvore = 0;
    char *arquivo = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--arvore") == 0 || strcmp(argv[i], "-a") == 0) {
            mostrar_arvore = 1;
        } else if (argv[i][0] != '-') {
            arquivo = argv[i];
        }
    }

    if (arquivo) {
        yyin = fopen(arquivo, "r");
        if (!yyin) {
            fprintf(stderr, "ERRO: não foi possível abrir o arquivo '%s'\n", arquivo);
            return 1;
        }
    } else {
        /* Lê da entrada padrão se nenhum arquivo for passado */
        yyin = stdin;
    }

    /* Análise léxica e sintática -> monta a AST em 'raiz' */
    if (yyparse() != 0) {
        return 1;
    }

    if (mostrar_arvore) {
        printf("===== ÁRVORE SINTÁTICA ABSTRATA (AST) =====\n");
        imprimir_arvore(raiz);
        printf("\n===== EXECUÇÃO =====\n");
    }

    /* Interpretação / Execução da AST */
    interpretar(raiz);

    if (arquivo) {
        fclose(yyin);
    }

    return 0;
}
