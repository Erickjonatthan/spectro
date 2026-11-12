/*
 * arvore.c
 * Funções para CRIAR os nós da árvore e para IMPRIMIR a árvore.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"

/* Cria um nó vazio do tipo pedido (calloc já zera todos os campos) */
No *novo_no(TipoNo tipo) {
    No *no = calloc(1, sizeof(No));
    no->tipo = tipo;
    no->tipo_dado = TIPO_NENHUM;
    return no;
}

No *criar_numero(double valor) {
    No *no = novo_no(NO_NUMERO);
    no->valor_num = valor;
    return no;
}

No *criar_variavel(char *nome) {
    No *no = novo_no(NO_VARIAVEL);
    no->nome = nome;
    return no;
}

No *criar_texto(char *texto) {
    No *no = novo_no(NO_TEXTO);
    no->texto = texto;
    return no;
}

No *criar_caractere(char valor) {
    No *no = novo_no(NO_CARACTERE);
    no->valor_char = valor;
    return no;
}

No *criar_bool(int valor) {
    No *no = novo_no(NO_BOOL);
    no->valor_bool = valor;
    return no;
}

No *criar_declaracao(char *nome, TipoDado tipo, No *expressao) {
    No *no = novo_no(NO_DECLARACAO);
    no->nome = nome;
    no->tipo_dado = tipo;
    no->expressao = expressao;
    return no;
}

No *criar_inicializacao(char *nome, TipoDado tipo) {
    No *no = novo_no(NO_INICIALIZACAO);
    no->nome = nome;
    no->tipo_dado = tipo;
    return no;
}

No *criar_atribuicao(char *nome, No *expressao) {
    No *no = novo_no(NO_ATRIBUICAO);
    no->nome = nome;
    no->expressao = expressao;
    return no;
}

No *criar_print(No *argumentos) {
    No *no = novo_no(NO_PRINT);
    no->argumentos = argumentos;
    return no;
}

No *criar_leitura(No *argumentos) {
    No *no = novo_no(NO_LEITURA);
    no->argumentos = argumentos;
    return no;
}

No *criar_operacao(TipoNo tipo, No *esquerda, No *direita) {
    No *no = novo_no(tipo);
    no->esquerda = esquerda;
    no->direita = direita;
    return no;
}

No *criar_unario(TipoNo tipo, No *expressao) {
    No *no = novo_no(tipo);
    no->expressao = expressao;
    return no;
}

No *criar_se(No *condicao, No *bloco_entao, No *bloco_senao) {
    No *no = novo_no(NO_SE);
    no->expressao = condicao;
    no->bloco_entao = bloco_entao;
    no->bloco_senao = bloco_senao;
    return no;
}

No *criar_ate(char *nome, TipoDado tipo, No *inicio, No *limite, No *corpo) {
    No *no = novo_no(NO_ATE);
    no->nome = nome;
    no->tipo_dado = tipo;
    no->inicio = inicio;
    no->expressao = limite;
    no->corpo = corpo;
    return no;
}

No *criar_ate_simples(No *condicao, No *corpo) {
    No *no = novo_no(NO_ATE);
    no->expressao = condicao;
    no->corpo = corpo;
    return no;
}

No *criar_enquanto(No *condicao, No *corpo) {
    No *no = novo_no(NO_ENQUANTO);
    no->expressao = condicao;
    no->corpo = corpo;
    return no;
}

No *criar_repete(No *corpo) {
    No *no = novo_no(NO_REPETE);
    no->corpo = corpo;
    return no;
}

No *criar_parar(void) {
    No *no = novo_no(NO_PARAR);
    return no;
}

No *criar_funcao(char *nome, int stopmeters, No *parametros, No *corpo) {
    No *no = novo_no(NO_FUNCAO);
    no->nome = nome;
    no->stopmeters = stopmeters;
    no->parametros = parametros;
    no->corpo = corpo;
    return no;
}

No *criar_chamada(char *nome, No *argumentos) {
    No *no = novo_no(NO_CHAMADA);
    no->nome = nome;
    no->argumentos = argumentos;
    return no;
}

No *criar_retorne(No *expressao) {
    No *no = novo_no(NO_RETORNE);
    no->expressao = expressao;
    return no;
}

No *criar_parametro(char *nome, TipoDado tipo) {
    No *no = novo_no(NO_PARAMETRO);
    no->nome = nome;
    no->tipo_dado = tipo;
    return no;
}

No *criar_acesso_trem(char *nome, No *indice) {
    No *no = novo_no(NO_ACESSO_TREM);
    no->nome = nome;
    no->expressao = indice;
    return no;
}

No *criar_atrib_trem(char *nome, No *indice, No *expressao) {
    No *no = novo_no(NO_ATRIB_TREM);
    no->nome = nome;
    no->esquerda = indice;
    no->expressao = expressao;
    return no;
}

No *criar_trem_literal(No *elementos) {
    No *no = novo_no(NO_TREM_LITERAL);
    no->argumentos = elementos;
    return no;
}

/* Coloca um comando no FINAL da lista de comandos do programa */
No *adicionar_comando(No *lista, No *comando) {
    if (lista == NULL)
        return comando;              /* lista vazia: o comando é o primeiro */

    No *atual = lista;
    while (atual->proximo != NULL)   /* anda até o último comando */
        atual = atual->proximo;
    atual->proximo = comando;
    return lista;
}

/* ===================== Impressão da árvore ===================== */

/* Imprime espaços de indentação */
void indentar(int nivel) {
    for (int i = 0; i < nivel; i++)
        printf("  ");
}

/* Imprime uma expressão com recuo */
void imprimir_expressao(No *no, int nivel) {
    if (no == NULL)
        return;

    indentar(nivel);

    if (no->tipo == NO_NUMERO) {
        if (no->valor_num == (int)no->valor_num)
            printf("NUMERO %d\n", (int)no->valor_num);
        else
            printf("NUMERO %.2f\n", no->valor_num);
    }
    else if (no->tipo == NO_VARIAVEL)
        printf("VARIAVEL %s\n", no->nome);
    else if (no->tipo == NO_TEXTO)
        printf("TEXTO \"%s\"\n", no->texto);
    else if (no->tipo == NO_CARACTERE)
        printf("CARACTERE '%c'\n", no->valor_char);
    else if (no->tipo == NO_BOOL)
        printf("BOOL %s\n", no->valor_bool ? "vdd" : "fls");
    else if (no->tipo == NO_SOMA) {
        printf("SOMA\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_SUBTRACAO) {
        printf("SUBTRACAO\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_MULTIPLICACAO) {
        printf("MULTIPLICACAO\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_DIVISAO) {
        printf("DIVISAO\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_NEGATIVO) {
        printf("NEGATIVO\n");
        imprimir_expressao(no->expressao, nivel + 1);
    }
    else if (no->tipo == NO_IGUAL) {
        printf("IGUAL\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_MAIOR) {
        printf("MAIOR\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_MENOR) {
        printf("MENOR\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_MAIOR_IGUAL) {
        printf("MAIOR_IGUAL\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_MENOR_IGUAL) {
        printf("MENOR_IGUAL\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_E) {
        printf("E_LOGICO\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_OU) {
        printf("OU_LOGICO\n");
        imprimir_expressao(no->esquerda, nivel + 1);
        imprimir_expressao(no->direita, nivel + 1);
    }
    else if (no->tipo == NO_NAO) {
        printf("NAO\n");
        imprimir_expressao(no->expressao, nivel + 1);
    }
    else if (no->tipo == NO_CHAMADA) {
        printf("CHAMADA %s\n", no->nome);
        No *arg = no->argumentos;
        while (arg) {
            imprimir_expressao(arg, nivel + 1);
            arg = arg->proximo;
        }
    }
    else if (no->tipo == NO_ACESSO_TREM) {
        printf("ACESSO_TREM %s\n", no->nome);
        imprimir_expressao(no->expressao, nivel + 1);
    }
    else if (no->tipo == NO_TREM_LITERAL) {
        printf("TREM_LITERAL\n");
        No *elem = no->argumentos;
        while (elem) {
            imprimir_expressao(elem, nivel + 1);
            elem = elem->proximo;
        }
    }
    else {
        printf("EXPRESSAO_DESCONHECIDA\n");
    }
}

/* Percorre a lista de comandos e imprime cada um */
void imprimir_no(No *cmd, int nivel) {
    if (cmd == NULL) return;

    indentar(nivel);

    if (cmd->tipo == NO_DECLARACAO) {
        printf("DECLARACAO %s (tipo: %s)\n", cmd->nome,
               cmd->tipo_dado == TIPO_RS ? "R$" :
               cmd->tipo_dado == TIPO_BOOLEANO ? "bool" :
               cmd->tipo_dado == TIPO_TEXTO_T ? "txt" :
               cmd->tipo_dado == TIPO_CARACTERE_T ? "abc" :
               cmd->tipo_dado == TIPO_TREM_T ? "trem" : "?");
        imprimir_expressao(cmd->expressao, nivel + 1);
    }
    else if (cmd->tipo == NO_INICIALIZACAO) {
        printf("INICIALIZACAO %s (tipo: %s)\n", cmd->nome,
               cmd->tipo_dado == TIPO_RS ? "R$" :
               cmd->tipo_dado == TIPO_BOOLEANO ? "bool" :
               cmd->tipo_dado == TIPO_TEXTO_T ? "txt" :
               cmd->tipo_dado == TIPO_CARACTERE_T ? "abc" :
               cmd->tipo_dado == TIPO_TREM_T ? "trem" : "?");
    }
    else if (cmd->tipo == NO_ATRIBUICAO) {
        printf("ATRIBUICAO %s\n", cmd->nome);
        imprimir_expressao(cmd->expressao, nivel + 1);
    }
    else if (cmd->tipo == NO_ATRIB_TREM) {
        printf("ATRIB_TREM %s\n", cmd->nome);
        indentar(nivel + 1);
        printf("INDICE:\n");
        imprimir_expressao(cmd->esquerda, nivel + 2);
        indentar(nivel + 1);
        printf("VALOR:\n");
        imprimir_expressao(cmd->expressao, nivel + 2);
    }
    else if (cmd->tipo == NO_PRINT) {
        printf("SS\n");
        No *arg = cmd->argumentos;
        while (arg) {
            imprimir_expressao(arg, nivel + 1);
            arg = arg->proximo;
        }
    }
    else if (cmd->tipo == NO_LEITURA) {
        printf("INPUT\n");
        No *arg = cmd->argumentos;
        while (arg) {
            imprimir_expressao(arg, nivel + 1);
            arg = arg->proximo;
        }
    }
    else if (cmd->tipo == NO_SE) {
        printf("SE\n");
        indentar(nivel + 1);
        printf("CONDICAO:\n");
        imprimir_expressao(cmd->expressao, nivel + 2);
        indentar(nivel + 1);
        printf("ENTAO:\n");
        for (No *c = cmd->bloco_entao; c; c = c->proximo)
            imprimir_no(c, nivel + 2);
        if (cmd->bloco_senao) {
            indentar(nivel + 1);
            printf("SENAO:\n");
            for (No *c = cmd->bloco_senao; c; c = c->proximo)
                imprimir_no(c, nivel + 2);
        }
    }
    else if (cmd->tipo == NO_ATE) {
        printf("ATE");
        if (cmd->nome) printf(" %s", cmd->nome);
        printf("\n");
        if (cmd->inicio) {
            indentar(nivel + 1);
            printf("INICIO:\n");
            imprimir_expressao(cmd->inicio, nivel + 2);
        }
        indentar(nivel + 1);
        printf("CONDICAO:\n");
        imprimir_expressao(cmd->expressao, nivel + 2);
        indentar(nivel + 1);
        printf("CORPO:\n");
        for (No *c = cmd->corpo; c; c = c->proximo)
            imprimir_no(c, nivel + 2);
    }
    else if (cmd->tipo == NO_ENQUANTO) {
        printf("ENQUANTO\n");
        indentar(nivel + 1);
        printf("CONDICAO:\n");
        imprimir_expressao(cmd->expressao, nivel + 2);
        indentar(nivel + 1);
        printf("CORPO:\n");
        for (No *c = cmd->corpo; c; c = c->proximo)
            imprimir_no(c, nivel + 2);
    }
    else if (cmd->tipo == NO_REPETE) {
        printf("REPETE\n");
        indentar(nivel + 1);
        printf("CORPO:\n");
        for (No *c = cmd->corpo; c; c = c->proximo)
            imprimir_no(c, nivel + 2);
    }
    else if (cmd->tipo == NO_PARAR)
        printf("PARE\n");
    else if (cmd->tipo == NO_RETORNE) {
        printf("RETORNE\n");
        if (cmd->expressao)
            imprimir_expressao(cmd->expressao, nivel + 1);
    }
    else if (cmd->tipo == NO_FUNCAO) {
        printf("FUNCAO %s%s\n", cmd->nome,
               cmd->stopmeters ? " (stopmeters)" : "");
        if (cmd->parametros) {
            indentar(nivel + 1);
            printf("PARAMETROS:\n");
            for (No *p = cmd->parametros; p; p = p->proximo) {
                indentar(nivel + 2);
                printf("PARAM %s (tipo: %s)\n", p->nome,
                       p->tipo_dado == TIPO_RS ? "R$" :
                       p->tipo_dado == TIPO_BOOLEANO ? "bool" :
                       p->tipo_dado == TIPO_TEXTO_T ? "txt" :
                       p->tipo_dado == TIPO_CARACTERE_T ? "abc" :
                       p->tipo_dado == TIPO_TREM_T ? "trem" : "?");
            }
        }
        indentar(nivel + 1);
        printf("CORPO:\n");
        for (No *c = cmd->corpo; c; c = c->proximo)
            imprimir_no(c, nivel + 2);
    }
    else if (cmd->tipo == NO_CHAMADA) {
        printf("CHAMADA %s\n", cmd->nome);
        No *arg = cmd->argumentos;
        while (arg) {
            imprimir_expressao(arg, nivel + 1);
            arg = arg->proximo;
        }
    }
    else {
        printf("COMANDO_DESCONHECIDO\n");
    }
}

void imprimir_arvore(No *programa) {
    for (No *cmd = programa; cmd != NULL; cmd = cmd->proximo) {
        imprimir_no(cmd, 0);
    }
}
