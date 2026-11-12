/*
 * arvore.h
 * Define a estrutura da Árvore Sintática Abstrata (AST) da linguagem Spectro.xum
 *
 * Cada construção da linguagem vira um "nó":
 *     10          -> nó NO_NUMERO
 *     x           -> nó NO_VARIAVEL
 *     x R$ {10}   -> nó NO_DECLARACAO
 *     ss(x)       -> nó NO_PRINT
 */
#ifndef ARVORE_H
#define ARVORE_H

/* Todos os tipos de nó que a linguagem possui */
typedef enum {
    NO_NUMERO,          /* 42, 3.14                                */
    NO_VARIAVEL,        /* x                                       */
    NO_TEXTO,           /* #hello#                                 */
    NO_CARACTERE,       /* 'a'                                     */
    NO_BOOL,            /* vdd / fls                               */
    NO_DECLARACAO,      /* x R$ {10}                               */
    NO_INICIALIZACAO,   /* x R$  (sem valor)                       */
    NO_ATRIBUICAO,      /* x {expr}                                */
    NO_PRINT,           /* ss(...) / screenshot(...)                */
    NO_LEITURA,         /* input(...)                               */
    NO_SOMA,            /* +                                       */
    NO_SUBTRACAO,       /* -                                       */
    NO_MULTIPLICACAO,   /* *                                       */
    NO_DIVISAO,         /* /                                       */
    NO_NEGATIVO,        /* -x  (unário)                            */
    NO_IGUAL,           /* ==                                      */
    NO_MAIOR_IGUAL,     /* =>                                      */
    NO_MENOR_IGUAL,     /* <=                                      */
    NO_MAIOR,           /* >>                                      */
    NO_MENOR,           /* <<                                      */
    NO_E,               /* &                                       */
    NO_OU,              /* |                                       */
    NO_NAO,             /* !                                       */
    NO_SE,              /* se (cond) ent % ... %                   */
    NO_ATE,             /* ate (i R$ 1 << 6) ent % ... %           */
    NO_ENQUANTO,        /* enquanto (cond) ent % ... %             */
    NO_REPETE,          /* repete % ... %                          */
    NO_PARAR,           /* pare                                    */
    NO_FUNCAO,          /* funsao nome(...) % ... %                */
    NO_CHAMADA,         /* nome(...)                               */
    NO_RETORNE,         /* retorne expr                            */
    NO_PARAMETRO,       /* parametro de funcao                     */
    NO_ACESSO_TREM,     /* lista[i]                                */
    NO_ATRIB_TREM,      /* lista[i] {val}                          */
    NO_TREM_LITERAL     /* [1, 2, 3]                               */
} TipoNo;

/* Tipos de dados da linguagem */
typedef enum {
    TIPO_RS,            /* R$   (número)      */
    TIPO_BOOLEANO,      /* bool               */
    TIPO_TEXTO_T,       /* txt                */
    TIPO_CARACTERE_T,   /* abc                */
    TIPO_TREM_T,        /* trem               */
    TIPO_NENHUM
} TipoDado;

/* Um único struct serve para todos os tipos de nó.
 * Cada tipo usa apenas os campos de que precisa. */
typedef struct No {
    TipoNo tipo;            /* que tipo de nó é este                    */
    double valor_num;       /* usado por NO_NUMERO                     */
    int valor_bool;         /* usado por NO_BOOL                       */
    char *nome;             /* usado por NO_DECLARACAO, NO_VARIAVEL... */
    char *texto;            /* usado por NO_TEXTO                      */
    char valor_char;        /* usado por NO_CARACTERE                  */
    TipoDado tipo_dado;     /* tipo de dado (R$, bool, txt, etc.)      */
    int stopmeters;         /* flag para funcoes stopmeters             */
    struct No *expressao;   /* valor da declaração, condição, etc.     */
    struct No *esquerda;    /* filho esquerdo (operações binárias)      */
    struct No *direita;     /* filho direito (operações binárias)       */
    struct No *bloco_entao; /* bloco do "se" (verdadeiro)               */
    struct No *bloco_senao; /* bloco do "se" (falso)                    */
    struct No *corpo;       /* corpo de laço ou função                  */
    struct No *parametros;  /* lista de parâmetros (função)             */
    struct No *argumentos;  /* lista de argumentos (chamada)            */
    struct No *inicio;      /* valor inicial do "ate"                   */
    struct No *proximo;     /* próximo comando (lista ligada)           */
} No;

/* Criação de nós */
No *criar_numero(double valor);
No *criar_variavel(char *nome);
No *criar_texto(char *texto);
No *criar_caractere(char valor);
No *criar_bool(int valor);
No *criar_declaracao(char *nome, TipoDado tipo, No *expressao);
No *criar_inicializacao(char *nome, TipoDado tipo);
No *criar_atribuicao(char *nome, No *expressao);
No *criar_print(No *argumentos);
No *criar_leitura(No *argumentos);
No *criar_operacao(TipoNo tipo, No *esquerda, No *direita);
No *criar_unario(TipoNo tipo, No *expressao);
No *criar_se(No *condicao, No *bloco_entao, No *bloco_senao);
No *criar_ate(char *nome, TipoDado tipo, No *inicio, No *limite, No *corpo);
No *criar_ate_simples(No *condicao, No *corpo);
No *criar_enquanto(No *condicao, No *corpo);
No *criar_repete(No *corpo);
No *criar_parar(void);
No *criar_funcao(char *nome, int stopmeters, No *parametros, No *corpo);
No *criar_chamada(char *nome, No *argumentos);
No *criar_retorne(No *expressao);
No *criar_parametro(char *nome, TipoDado tipo);
No *criar_acesso_trem(char *nome, No *indice);
No *criar_atrib_trem(char *nome, No *indice, No *expressao);
No *criar_trem_literal(No *elementos);
No *adicionar_comando(No *lista, No *comando);

/* Impressão e interpretação */
void imprimir_arvore(No *programa);
void interpretar(No *programa);

#endif
