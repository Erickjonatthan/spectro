%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* O arquivo parser.tab.h será gerado automaticamente pelo Bison */
#include "parser.tab.h" 
%}

/* noyywrap: apenas um arquivo de entrada.
   yylineno: o Flex conta as linhas automaticamente para mensagens de erro */
%option noyywrap yylineno

%%

/* --- PALAVRAS RESERVADAS / COMANDOS --- */
"se"            { return SE; }
"senao"         { return SENAO; }
"ent"           { return ENT; }
"funsao"        { return FUNSAO; }
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

/* --- IDENTIFICADORES e LITERAIS --- */
/* Identificador: começa com letra, seguido por letras, números ou '_' */
[a-zA-Z][a-zA-Z0-9_]*   { yylval.nome = strdup(yytext); return IDENTIFICADOR; }

/* Número: Inteiro ou Real (ex: 7 ou 3.14) */
[0-9]+(\.[0-9]+)?       { yylval.valor_num = atof(yytext); return NUMERO; }

/* Texto: qualquer coisa entre caracteres '#' */
#[^#]*#                 { 
                          // Remove os '#' das pontas se quiser salvar apenas o conteúdo
                          yylval.texto = strdup(yytext); 
                          return TEXTO; 
                        }

/* Caractere único: apenas uma letra/símbolo (o documento não detalhou se usa aspas, assumindo literal por enquanto) */
"'"."'"                 { yylval.caractere = yytext[1]; return CARACTERE; }


/* --- OPERADORES ARITMÉTICOS --- */
"+"             { return SOMAR; }
"-"             { return SUBTRAIR; }
*"             { return MULTIPLICAR; }
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

/* --- DELIMITADORES --- */
"("             { return ABRE_PARENTESES; }
")"             { return FECHA_PARENTESES; }
"{"             { return ABRE_CHAVES; }
"}"             { return FECHA_CHAVES; }
"["             { return ABRE_COLCHETES; }
"]"             { return FECHA_COLCHETES; }
"%"             { return PORCENTAGEM; } /* Usado para blocos de código */
","             { return VIRGULA; }

/* --- COMENTÁRIOS --- */
"?"[^?\n]*              { /* Comentário de uma linha: ignora até o fim da linha */ }
"\?\?"([^*]|\?)*"\?\?"  { /* Comentário multilinha ?? ... ?? : ignora */ }

/* --- ESPAÇOS E QUEBRAS DE LINHA --- */
[ \t\r]+        { /* Espaços e tabs horizontais são ignorados */ }
"\n"            { return FIM_LINHA; } /* A quebra de linha termina o comando! */

.               { printf("ERRO léxico na linha %d: caractere '%s' desconhecido\n", yylineno, yytext); exit(1); }

%%
