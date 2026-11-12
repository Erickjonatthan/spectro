%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"
/* Arquivo de tokens gerado pelo Bison a partir de sintatico.y */
#include "sintatico.tab.h"
%}

/* noyywrap: leitura de arquivo único
   yylineno: rastreamento automático do número da linha para mensagens de erro */
%option noyywrap yylineno

%%

    /* --- PALAVRAS RESERVADAS E COMANDOS --- */
"se"            { return SE; }
"senao"         { return SENAO; }
"ent"           { return ENT; }
"funsao"        { return FUNSAO; }
"stopmeters"    { return STOPMETERS; }
"retorne"       { return RETORNE; }
"ate"           { return ATE; }
"enquanto"      { return ENQUANTO; }
"repete"        { return REPETE; }
"pare"          { return PARE; }
"ss"            { return SS; }
"screenshot"    { return SCREENSHOT; }
"input"         { return INPUT; }

    /* --- TIPOS DE DADOS --- */
"R$"            { return TIPO_NUMERO; }
"bool"          { return TIPO_BOOL; }
"txt"           { return TIPO_TXT; }
"abc"           { return TIPO_ABC; }
"trem"          { return TIPO_TREM; }

    /* --- VALORES BOOLEANOS --- */
"vdd"           { yylval.valor_bool = 1; return VAL_BOOL; }
"fls"           { yylval.valor_bool = 0; return VAL_BOOL; }

    /* --- IDENTIFICADORES E LITERAIS --- */
    /* Começa com letra (case-sensitive), seguido por letras, números ou '_' */
[a-zA-Z][a-zA-Z0-9_]*   { yylval.nome = strdup(yytext); return IDENTIFICADOR; }

    /* Número inteiro ou real */
[0-9]+(\.[0-9]+)?       { yylval.valor_num = atof(yytext); return NUMERO; }

    /* Texto delimitado por '#' (armazena o conteúdo sem os '#' das pontas) */
#[^#\n]*#               {
                          char *conteudo = strdup(yytext + 1);
                          conteudo[strlen(conteudo) - 1] = '\0';
                          yylval.texto = conteudo;
                          return TEXTO;
                        }

    /* Erro específico se abrir '#' e quebrar a linha sem fechar */
#[^#\n]*                {
                          printf("ERRO léxico na linha %d: texto não fechado com '#'\n", yylineno);
                          exit(1);
                        }

    /* Caractere único entre aspas simples */
"'"."'"                 { yylval.caractere = yytext[1]; return CARACTERE; }

    /* --- OPERADORES ARITMÉTICOS --- */
"+"             { return SOMAR; }
"-"             { return SUBTRAIR; }
"*"             { return MULTIPLICAR; }
"/"             { return DIVIDIR; }

    /* --- OPERADORES RELACIONAIS E LÓGICOS --- */
"=="            { return VERIFICAR_IGUAL; }
"=>"            { return VERIFICAR_MAIORIGUAL; }
"<="            { return VERIFICAR_MENORIGUAL; }
">>"            { return VERIFICAR_MAIOR; }
"<<"            { return VERIFICAR_MENOR; }
"&"             { return OP_E; }
"|"             { return OP_OU; }
"!"             { return OP_NAO; }

    /* --- DELIMITADORES E PONTUAÇÃO --- */
"("             { return ABRE_PARENTESES; }
")"             { return FECHA_PARENTESES; }
"{"             { return ABRE_CHAVES; }
"}"             { return FECHA_CHAVES; }
"["             { return ABRE_COLCHETES; }
"]"             { return FECHA_COLCHETES; }
"%"             { return PORCENTAGEM; }
","             { return VIRGULA; }

    /* --- COMENTÁRIOS --- */
    /* Multilinha (?? ... ??) deve vir antes do comentário de linha única (?) */
\?\?([^?]|\?[^?])*\?\?  { /* Ignora comentário de bloco */ }
\?[^\n]*                { /* Ignora comentário de linha única */ }

    /* --- ESPAÇOS E QUEBRAS DE LINHA --- */
[ \t\r]+        { /* Ignora espaços e tabulações */ }
"\n"            { return FIM_LINHA; }

    /* --- CARACTERE INVÁLIDO --- */
.               {
                  printf("ERRO léxico na linha %d: caractere '%s' desconhecido\n", yylineno, yytext);
                  exit(1);
                }

%%