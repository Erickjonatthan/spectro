/*
 * sintatico.y — ANALISADOR SINTÁTICO (Bison)
 *
 * Função: receber os tokens do Flex, verificar a gramática
 * e CONSTRUIR A ÁRVORE (AST).
 * Quem executa é o interpretador (interpretador.c).
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"

int yylex(void);
extern int yylineno;
extern FILE *yyin;

void yyerror(const char *mensagem);

No *raiz = NULL;   /* Raiz da AST completa do programa */
%}

%define parse.error verbose

%union {
    double valor_num;
    int valor_bool;
    char *texto;
    char *nome;
    char caractere;
    int tipo_dado;
    No *no;
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

/* --- TIPOS DE REGRAS --- */
%type <tipo_dado> tipo
%type <no> programa lista_instrucoes instrucao comando_simples comando_composto bloco
%type <no> declaracao_var atribuicao comando_ss comando_input comando_retorne comando_pare chamada_funcao
%type <no> comando_se comando_ate comando_enquanto comando_repete declaracao_funcao
%type <no> parametro lista_parametros lista_parametros_opc
%type <no> expressao lista_expressoes lista_expressoes_opc

%start programa

%%

/* 1. ESTRUTURA GERAL DO PROGRAMA E QUEBRAS DE LINHA */

programa:
    quebras_opc lista_instrucoes     { raiz = $2; $$ = $2; }
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
    /* vazio */                     { $$ = NULL; }
    | lista_instrucoes instrucao    { $$ = adicionar_comando($1, $2); }
    ;

instrucao:
    comando_simples quebras         { $$ = $1; }
    | comando_composto              { $$ = $1; }
    ;

/* 2. COMANDOS SIMPLES (terminam com quebra de linha) */

comando_simples:
    declaracao_var                  { $$ = $1; }
    | atribuicao                    { $$ = $1; }
    | comando_ss                    { $$ = $1; }
    | comando_input                 { $$ = $1; }
    | comando_retorne               { $$ = $1; }
    | comando_pare                  { $$ = $1; }
    | chamada_funcao                { $$ = $1; }
    ;

/* Ex: idade R$   ou   soma R$ { 2000 + 200 } */
declaracao_var:
    IDENTIFICADOR tipo                                          { $$ = criar_inicializacao($1, (TipoDado)$2); }
    | IDENTIFICADOR tipo ABRE_CHAVES expressao FECHA_CHAVES     { $$ = criar_declaracao($1, (TipoDado)$2, $4); }
    ;

tipo:
    TIPO_NUMERO                     { $$ = TIPO_RS; }
    | TIPO_BOOL                     { $$ = TIPO_BOOLEANO; }
    | TIPO_TXT                      { $$ = TIPO_TEXTO_T; }
    | TIPO_ABC                      { $$ = TIPO_CARACTERE_T; }
    | TIPO_TREM                     { $$ = TIPO_TREM_T; }
    ;

/* Ex: total { total + i }   ou   lista[0] { 99 } */
atribuicao:
    IDENTIFICADOR ABRE_CHAVES expressao FECHA_CHAVES                                            { $$ = criar_atribuicao($1, $3); }
    | IDENTIFICADOR ABRE_COLCHETES expressao FECHA_COLCHETES ABRE_CHAVES expressao FECHA_CHAVES { $$ = criar_atrib_trem($1, $3, $6); }
    ;

/* Ex: ss(#Resultado: #, resultado)   ou   screenshot(x) */
comando_ss:
    SS ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES            { $$ = criar_print($3); }
    | SCREENSHOT ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES  { $$ = criar_print($3); }
    ;

/* Ex: input(idade)   ou   input(#Digite: #, idade) */
comando_input:
    INPUT ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES        { $$ = criar_leitura($3); }
    ;

/* Ex: retorne n * 2   ou   retorne */
comando_retorne:
    RETORNE expressao               { $$ = criar_retorne($2); }
    | RETORNE                       { $$ = criar_retorne(NULL); }
    ;

/* Ex: pare */
comando_pare:
    PARE                            { $$ = criar_parar(); }
    ;

/* Ex: dobro(7) */
chamada_funcao:
    IDENTIFICADOR ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES { $$ = criar_chamada($1, $3); }
    ;

/* 3. COMANDOS COMPOSTOS E BLOCOS (% ... %) */

comando_composto:
    comando_se                      { $$ = $1; }
    | comando_ate                   { $$ = $1; }
    | comando_enquanto              { $$ = $1; }
    | comando_repete                { $$ = $1; }
    | declaracao_funcao             { $$ = $1; }
    ;

bloco:
    PORCENTAGEM quebras_opc lista_instrucoes PORCENTAGEM               { $$ = $3; }
    ;

/* Condicionais: se ... ent % ... % [senao % ... % | senao se ...] */
comando_se:
    SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras
        { $$ = criar_se($3, $6, NULL); }
    | SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras SENAO bloco quebras
        { $$ = criar_se($3, $6, $9); }
    | SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco SENAO bloco quebras
        { $$ = criar_se($3, $6, $8); }
    | SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras SENAO comando_se
        { $$ = criar_se($3, $6, $9); }
    | SE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco SENAO comando_se
        { $$ = criar_se($3, $6, $8); }
    ;

/* Laço ate: ex: ate (i R$ 1 << 6) ent % ... %   ou   ate (x << 10) ent % ... % */
comando_ate:
    ATE ABRE_PARENTESES IDENTIFICADOR tipo expressao VERIFICAR_MENOR expressao FECHA_PARENTESES ENT bloco quebras
        { $$ = criar_ate($3, (TipoDado)$4, $5, criar_operacao(NO_MENOR, criar_variavel($3), $7), $10); }
    | ATE ABRE_PARENTESES IDENTIFICADOR tipo expressao FECHA_PARENTESES ENT bloco quebras
        { $$ = criar_ate($3, (TipoDado)$4, $5, $5, $8); }
    | ATE ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras
        { $$ = criar_ate_simples($3, $6); }
    ;

/* Laço enquanto: ex: enquanto (x << 10) ent % ... % */
comando_enquanto:
    ENQUANTO ABRE_PARENTESES expressao FECHA_PARENTESES ENT bloco quebras
        { $$ = criar_enquanto($3, $6); }
    | ENQUANTO ABRE_PARENTESES expressao FECHA_PARENTESES bloco quebras
        { $$ = criar_enquanto($3, $5); }
    ;

/* Laço repete (interrompido com 'pare') */
comando_repete:
    REPETE bloco quebras            { $$ = criar_repete($2); }
    ;

/* Funções: ex: funsao dobro(n R$) % ... %   ou   funsao dobro stopmeters (n R$) % ... % */
declaracao_funcao:
    FUNSAO IDENTIFICADOR ABRE_PARENTESES lista_parametros_opc FECHA_PARENTESES bloco quebras
        { $$ = criar_funcao($2, 0, $4, $6); }
    | FUNSAO IDENTIFICADOR STOPMETERS ABRE_PARENTESES lista_parametros_opc FECHA_PARENTESES bloco quebras
        { $$ = criar_funcao($2, 1, $5, $7); }
    ;

lista_parametros_opc:
    /* vazio */                     { $$ = NULL; }
    | lista_parametros              { $$ = $1; }
    ;

lista_parametros:
    parametro                       { $$ = $1; }
    | lista_parametros VIRGULA parametro { $$ = adicionar_comando($1, $3); }
    ;

parametro:
    IDENTIFICADOR tipo              { $$ = criar_parametro($1, (TipoDado)$2); }
    ;

/* 4. EXPRESSÕES E LISTAS (trem) */

lista_expressoes_opc:
    /* vazio */                     { $$ = NULL; }
    | lista_expressoes              { $$ = $1; }
    ;

lista_expressoes:
    expressao                       { $$ = $1; }
    | lista_expressoes VIRGULA expressao { $$ = adicionar_comando($1, $3); }
    ;

expressao:
    NUMERO                          { $$ = criar_numero($1); }
    | TEXTO                         { $$ = criar_texto($1); }
    | CARACTERE                     { $$ = criar_caractere($1); }
    | VAL_BOOL                      { $$ = criar_bool($1); }
    | IDENTIFICADOR                 { $$ = criar_variavel($1); }
    | IDENTIFICADOR ABRE_COLCHETES expressao FECHA_COLCHETES { $$ = criar_acesso_trem($1, $3); }
    | chamada_funcao                { $$ = $1; }
    | INPUT                         { $$ = criar_leitura(NULL); }
    | INPUT ABRE_PARENTESES lista_expressoes_opc FECHA_PARENTESES { $$ = criar_leitura($3); }
    | ABRE_COLCHETES lista_expressoes_opc FECHA_COLCHETES         { $$ = criar_trem_literal($2); }
    | ABRE_PARENTESES expressao FECHA_PARENTESES                  { $$ = $2; }
    | SUBTRAIR expressao %prec UMINUS                             { $$ = criar_unario(NO_NEGATIVO, $2); }
    | OP_NAO expressao                                            { $$ = criar_unario(NO_NAO, $2); }
    | expressao SOMAR expressao                                   { $$ = criar_operacao(NO_SOMA, $1, $3); }
    | expressao SUBTRAIR expressao                                { $$ = criar_operacao(NO_SUBTRACAO, $1, $3); }
    | expressao MULTIPLICAR expressao                             { $$ = criar_operacao(NO_MULTIPLICACAO, $1, $3); }
    | expressao DIVIDIR expressao                                 { $$ = criar_operacao(NO_DIVISAO, $1, $3); }
    | expressao VERIFICAR_IGUAL expressao                         { $$ = criar_operacao(NO_IGUAL, $1, $3); }
    | expressao VERIFICAR_MAIORIGUAL expressao                    { $$ = criar_operacao(NO_MAIOR_IGUAL, $1, $3); }
    | expressao VERIFICAR_MENORIGUAL expressao                    { $$ = criar_operacao(NO_MENOR_IGUAL, $1, $3); }
    | expressao VERIFICAR_MAIOR expressao                         { $$ = criar_operacao(NO_MAIOR, $1, $3); }
    | expressao VERIFICAR_MENOR expressao                         { $$ = criar_operacao(NO_MENOR, $1, $3); }
    | expressao OP_E expressao                                    { $$ = criar_operacao(NO_E, $1, $3); }
    | expressao OP_OU expressao                                   { $$ = criar_operacao(NO_OU, $1, $3); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERRO sintático na linha %d: %s\n", yylineno, s);
    exit(1);
}
