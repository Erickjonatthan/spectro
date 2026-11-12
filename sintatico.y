%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex(void);
extern int yylineno;
extern FILE *yyin;

void yyerror(const char *s);
%}

/* Mensagens de erro sintático mais detalhadas no Bison */
%define parse.error verbose

/* Deve corresponder exatamente aos campos usados em yylval no lexico.l */
%union {
    double valor_num;
    int valor_bool;
    char *texto;
    char *nome;
    char caractere;
    int tipo_dado;
}

/* --- TOKENS SEM VALOR SEMÂNTICO --- */
%token SE SENAO ENT FUNSAO STOPMETERS RETORNE
%token ATE ENQUANTO REPETE PARE
%token SS SCREENSHOT INPUT
%token TIPO_NUMERO TIPO_BOOL TIPO_TXT TIPO_ABC TIPO_TREM

%token SOMAR SUBTRAIR MULTIPLICAR DIVIDIR
%token VERIFICAR_IGUAL VERIFICAR_MAIORIGUAL VERIFICAR_MENORIGUAL
%token VERIFICAR_MAIOR VERIFICAR_MENOR
%token OP_E OP_OU OP_NAO

%token ABRE_PARENTESES FECHA_PARENTESES
%token ABRE_CHAVES FECHA_CHAVES
%token ABRE_COLCHETES FECHA_COLCHETES
%token PORCENTAGEM VIRGULA
%token FIM_LINHA

/* --- TOKENS COM VALOR SEMÂNTICO --- */
%token <valor_bool> VAL_BOOL
%token <nome> IDENTIFICADOR
%token <valor_num> NUMERO
%token <texto> TEXTO
%token <caractere> CARACTERE

/* --- PRECEDÊNCIA E ASSOCIATIVIDADE (da menor para a maior) --- */
%left OP_OU
%left OP_E
%right OP_NAO
%left VERIFICAR_IGUAL VERIFICAR_MAIORIGUAL VERIFICAR_MENORIGUAL VERIFICAR_MAIOR VERIFICAR_MENOR
%left SOMAR SUBTRAIR
%left MULTIPLICAR DIVIDIR
%right UMINUS

%start programa

%%

/* 1. ESTRUTURA GERAL DO PROGRAMA E QUEBRAS DE LINHA */

programa:
    quebras_opc lista_instrucoes
    ;

quebras:
    FIM_LINHA
    | quebras FIM_LINHA
    ;

quebras_opc:
    /* vazio */
    | quebras
    ;

lista_instrucoes:
    /* vazio */
    | lista_instrucoes instrucao
    ;

instrucao:
    comando_simples quebras
    | comando_composto
    ;

/* 2. COMANDOS SIMPLES (terminam com quebra de linha) */

comando_simples:
    declaracao_var
    | atribuicao
    | comando_ss
    | comando_input
    | comando_retorne
    | comando_pare
    | chamada_funcao
    ;

/* Ex: idade R$   ou   soma R$ { 2000 + 200 } */
declaracao_var:
    IDENTIFICADOR tipo
    | IDENTIFICADOR tipo ABRE_CHAVES expressao FECHA_CHAVES
    ;

tipo:
    TIPO_NUMERO
    | TIPO_BOOL
    | TIPO_TXT
    | TIPO_ABC
    | TIPO_TREM
    ;

/* Ex: total { total + i }   ou   lista[0] { 99 } */
atribuicao:
    IDENTIFICADOR ABRE_CHAVES expressao FECHA_CHAVES
    | IDENTIFICADOR ABRE_COLCHETES expressao FECHA_COLCHETES ABRE_CHAVES expressao FECHA_CHAVES
    ;

/* Ex: ss(#Resultado: #, resultado)   ou   screenshot(x) */
comando_ss:
    SS ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES
    | SCREENSHOT ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES
    ;

/* Ex: input(idade)   ou   input(#Digite: #, idade) */
comando_input:
    INPUT ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES
    ;

/* Ex: retorne n * 2   ou   retorne */
comando_retorne:
    RETORNE expressao
    | RETORNE
    ;

/* Ex: pare */
comando_pare:
    PARE
    ;

/* Ex: dobro(7) */
chamada_funcao:
    IDENTIFICADOR ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES
    ;

/* 3. COMANDOS COMPOSTOS E BLOCOS (% ... %) */

comando_composto:
    comando_se
    | comando_ate
    | comando_enquanto
    | comando_repete
    | declaracao_funcao
    ;

bloco:
    PORCENTAGEM quebras_opc lista_instrucoes PORCENTAGEM
    ;

/* Condicionais: se ... ent % ... % [senao % ... % | senao se ...] */
comando_se:
    SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras
    | SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras SENAO bloco quebras
    | SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco SENAO bloco quebras
    | SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras SENAO comando_se
    | SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco SENAO comando_se
    ;

/* Laço ate: ex: ate (i R$ 1 << 6) ent % ... %   ou   ate (x << 10) ent % ... % */
comando_ate:
    ATE ABRE_PARENTESES IDENTIFICADOR tipo expressao FECHA_PARENTESES ENT bloco quebras
    | ATE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras
    ;

/* Laço enquanto: ex: enquanto (x << 10) ent % ... % */
comando_enquanto:
    ENQUANTO ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras
    | ENQUANTO ABRE_PARENTESES expressao FECHA_PARENTESES bloco quebras
    ;

/* Laço repete (interrompido com 'pare') */
comando_repete:
    REPETE bloco quebras
    ;

/* Funções: ex: funsao dobro(n R$) % ... %   ou   funsao dobro stopmeters (n R$) % ... % */
declaracao_funcao:
    FUNSAO IDENTIFICADOR ABRE_PARENTESES lista_parametros_opc FECHA_PARENTESES bloco quebras
    | FUNSAO IDENTIFICADOR STOPMETERS ABRE_PARENTESES lista_parametros_opc FECHA_PARENTESES bloco quebras
    ;

lista_parametros_opc:
    /* vazio */
    | lista_parametros
    ;

lista_parametros:
    parametro
    | lista_parametros VIRGULA parametro
    ;

parametro:
    IDENTIFICADOR tipo
    ;

/* 4. EXPRESSÕES E LISTAS (trem) */

lista_expressoes_opc:
    /* vazio */
    | lista_expressoes
    ;

lista_expressoes:
    expressao
    | lista_expressoes VIRGULA expressao
    ;

expressao:
    NUMERO
    | TEXTO
    | CARACTERE
    | VAL_BOOL
    | IDENTIFICADOR
    | IDENTIFICADOR ABRE_COLCHETES expressao FECHA_COLCHETES
    | chamada_funcao
    | INPUT ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES
    | ABRE_COLCHETES lista_expressoes_opc FECHA_COLCHETES
    | ABRE_PARENTESES expressao FECHA_PARENTESES
    | SUBTRAIR expressao %prec UMINUS
    | OP_NAO expressao
    | expressao SOMAR expressao
    | expressao SUBTRAIR expressao
    | expressao MULTIPLICAR expressao
    | expressao DIVIDIR expressao
    | expressao VERIFICAR_IGUAL expressao
    | expressao VERIFICAR_MAIORIGUAL expressao
    | expressao VERIFICAR_MENORIGUAL expressao
    | expressao VERIFICAR_MAIOR expressao
    | expressao VERIFICAR_MENOR expressao
    | expressao OP_E expressao
    | expressao OP_OU expressao
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERRO sintático na linha %d: %s\n", yylineno, s);
    exit(1);
}