/*
 * interpretador.c — INTERPRETADOR
 *
 * Função: EXECUTAR o programa percorrendo a árvore construída pelo parser.
 * Baseado no modelo de interpretador com tabela de símbolos, seguindo
 * o estilo simples e direto do repositório de aulas.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "arvore.h"

#define MAX_VARIAVEIS 256
#define MAX_TREM_TAM 128
#define MAX_FUNCOES 64

/* Estrutura para armazenar valor em tempo de execução */
typedef struct Valor {
    TipoDado tipo;
    double numero;
    int booleano;
    char *texto;
    char caractere;
    /* Para trem */
    struct Valor *elementos;
    int trem_tamanho;
} Valor;

/* Tabela de variáveis */
typedef struct {
    char *nome;
    TipoDado tipo_declarado;
    Valor val;
} Variavel;

static Variavel variaveis[MAX_VARIAVEIS];
static int total_variaveis = 0;

/* Tabela de funções */
typedef struct {
    char *nome;
    int stopmeters;
    No *parametros;
    No *corpo;
} FuncaoDef;

static FuncaoDef funcoes[MAX_FUNCOES];
static int total_funcoes = 0;

/* Controle de fluxo */
static int sinal_pare = 0;
static int sinal_retorne = 0;
static Valor valor_retorno;

/* Mensagem de erro padrão conforme especificação */
static void erro(const char *msg, const char *detalhe) {
    if (detalhe && strlen(detalhe) > 0) {
        printf("ERRO --> %s {%s}\n", msg, detalhe);
    } else {
        printf("ERRO --> %s\n", msg);
    }
    exit(1);
}

/* Busca variável na tabela */
static int buscar_variavel(const char *nome) {
    for (int i = 0; i < total_variaveis; i++) {
        if (strcmp(variaveis[i].nome, nome) == 0)
            return i;
    }
    return -1;
}

/* Busca função na tabela */
static int buscar_funcao(const char *nome) {
    for (int i = 0; i < total_funcoes; i++) {
        if (strcmp(funcoes[i].nome, nome) == 0)
            return i;
    }
    return -1;
}

/* Cria valor vazio/padrão */
static Valor valor_padrao(TipoDado tipo) {
    Valor v;
    memset(&v, 0, sizeof(v));
    v.tipo = tipo;
    if (tipo == TIPO_TEXTO_T) {
        v.texto = strdup("");
    } else if (tipo == TIPO_TREM_T) {
        v.elementos = (Valor *)calloc(MAX_TREM_TAM, sizeof(Valor));
        v.trem_tamanho = 0;
    }
    return v;
}

static Valor copiar_valor(Valor v) {
    Valor novo = v;
    if (v.tipo == TIPO_TEXTO_T && v.texto) {
        novo.texto = strdup(v.texto);
    } else if (v.tipo == TIPO_TREM_T && v.elementos) {
        novo.elementos = (Valor *)calloc(MAX_TREM_TAM, sizeof(Valor));
        novo.trem_tamanho = v.trem_tamanho;
        for (int i = 0; i < v.trem_tamanho; i++) {
            novo.elementos[i] = copiar_valor(v.elementos[i]);
        }
    }
    return novo;
}

/* Converte valor para número */
static double para_numero(Valor v) {
    if (v.tipo == TIPO_RS) return v.numero;
    if (v.tipo == TIPO_BOOLEANO) return (double)v.booleano;
    if (v.tipo == TIPO_CARACTERE_T) return (double)v.caractere;
    return 0.0;
}

/* Converte valor para booleano */
static int para_bool(Valor v) {
    if (v.tipo == TIPO_BOOLEANO) return v.booleano;
    if (v.tipo == TIPO_RS) return v.numero != 0.0;
    if (v.tipo == TIPO_TEXTO_T) return (v.texto && strlen(v.texto) > 0);
    return 0;
}

/* Imprime valor */
static void imprimir_valor(Valor v) {
    if (v.tipo == TIPO_RS) {
        if (v.numero == (int)v.numero)
            printf("%d", (int)v.numero);
        else
            printf("%.6g", v.numero);
    } else if (v.tipo == TIPO_BOOLEANO) {
        printf("%s", v.booleano ? "vdd" : "fls");
    } else if (v.tipo == TIPO_TEXTO_T) {
        printf("%s", v.texto ? v.texto : "");
    } else if (v.tipo == TIPO_CARACTERE_T) {
        printf("%c", v.caractere);
    } else if (v.tipo == TIPO_TREM_T) {
        printf("[");
        for (int i = 0; i < v.trem_tamanho; i++) {
            if (i > 0) printf(", ");
            imprimir_valor(v.elementos[i]);
        }
        printf("]");
    }
}

/* Declaração antecipada */
static Valor avaliar(No *no);
static void interpretar_bloco(No *bloco);

/* Avaliar expressões */
static Valor avaliar(No *no) {
    Valor v;
    memset(&v, 0, sizeof(v));
    if (!no) return v;

    switch (no->tipo) {
        case NO_NUMERO:
            v.tipo = TIPO_RS;
            v.numero = no->valor_num;
            return v;

        case NO_BOOL:
            v.tipo = TIPO_BOOLEANO;
            v.booleano = no->valor_bool;
            return v;

        case NO_TEXTO:
            v.tipo = TIPO_TEXTO_T;
            v.texto = strdup(no->texto ? no->texto : "");
            return v;

        case NO_CARACTERE:
            v.tipo = TIPO_CARACTERE_T;
            v.caractere = no->valor_char;
            return v;

        case NO_VARIAVEL: {
            int pos = buscar_variavel(no->nome);
            if (pos == -1)
                erro("Variável não encontrada", no->nome);
            return copiar_valor(variaveis[pos].val);
        }

        case NO_ACESSO_TREM: {
            int pos = buscar_variavel(no->nome);
            if (pos == -1)
                erro("Variável não encontrada", no->nome);
            if (variaveis[pos].tipo_declarado != TIPO_TREM_T)
                erro("Variável não é do tipo trem", no->nome);
            Valor idx_val = avaliar(no->expressao);
            int idx = (int)para_numero(idx_val);
            if (idx < 0 || idx >= variaveis[pos].val.trem_tamanho)
                erro("Índice fora do limite do trem", no->nome);
            return copiar_valor(variaveis[pos].val.elementos[idx]);
        }

        case NO_TREM_LITERAL: {
            v.tipo = TIPO_TREM_T;
            v.elementos = (Valor *)calloc(MAX_TREM_TAM, sizeof(Valor));
            v.trem_tamanho = 0;
            No *elem = no->argumentos;
            while (elem && v.trem_tamanho < MAX_TREM_TAM) {
                v.elementos[v.trem_tamanho++] = avaliar(elem);
                elem = elem->proximo;
            }
            return v;
        }

        case NO_SOMA: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            if (esq.tipo == TIPO_TEXTO_T || dir.tipo == TIPO_TEXTO_T) {
                /* Concatenação */
                char buf[2048] = "";
                if (esq.tipo == TIPO_TEXTO_T) strcat(buf, esq.texto);
                else {
                    char tmp[64];
                    snprintf(tmp, sizeof(tmp), "%.6g", para_numero(esq));
                    strcat(buf, tmp);
                }
                if (dir.tipo == TIPO_TEXTO_T) strcat(buf, dir.texto);
                else {
                    char tmp[64];
                    snprintf(tmp, sizeof(tmp), "%.6g", para_numero(dir));
                    strcat(buf, tmp);
                }
                v.tipo = TIPO_TEXTO_T;
                v.texto = strdup(buf);
                return v;
            }
            v.tipo = TIPO_RS;
            v.numero = para_numero(esq) + para_numero(dir);
            return v;
        }

        case NO_SUBTRACAO: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_RS;
            v.numero = para_numero(esq) - para_numero(dir);
            return v;
        }

        case NO_MULTIPLICACAO: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_RS;
            v.numero = para_numero(esq) * para_numero(dir);
            return v;
        }

        case NO_DIVISAO: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            double d = para_numero(dir);
            if (d == 0.0)
                erro("Operação inválida", "divisão por zero");
            v.tipo = TIPO_RS;
            v.numero = para_numero(esq) / d;
            return v;
        }

        case NO_NEGATIVO: {
            Valor expr = avaliar(no->expressao);
            v.tipo = TIPO_RS;
            v.numero = -para_numero(expr);
            return v;
        }

        case NO_IGUAL: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_BOOLEANO;
            if (esq.tipo == TIPO_TEXTO_T && dir.tipo == TIPO_TEXTO_T)
                v.booleano = (strcmp(esq.texto, dir.texto) == 0);
            else
                v.booleano = (para_numero(esq) == para_numero(dir));
            return v;
        }

        case NO_MAIOR_IGUAL: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_BOOLEANO;
            v.booleano = (para_numero(esq) >= para_numero(dir));
            return v;
        }

        case NO_MENOR_IGUAL: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_BOOLEANO;
            v.booleano = (para_numero(esq) <= para_numero(dir));
            return v;
        }

        case NO_MAIOR: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_BOOLEANO;
            v.booleano = (para_numero(esq) > para_numero(dir));
            return v;
        }

        case NO_MENOR: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_BOOLEANO;
            v.booleano = (para_numero(esq) < para_numero(dir));
            return v;
        }

        case NO_E: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_BOOLEANO;
            v.booleano = (para_bool(esq) && para_bool(dir));
            return v;
        }

        case NO_OU: {
            Valor esq = avaliar(no->esquerda);
            Valor dir = avaliar(no->direita);
            v.tipo = TIPO_BOOLEANO;
            v.booleano = (para_bool(esq) || para_bool(dir));
            return v;
        }

        case NO_NAO: {
            Valor expr = avaliar(no->expressao);
            v.tipo = TIPO_BOOLEANO;
            v.booleano = !para_bool(expr);
            return v;
        }

        case NO_CHAMADA: {
            int pos = buscar_funcao(no->nome);
            if (pos == -1)
                erro("Função não encontrada", no->nome);
            
            FuncaoDef *f = &funcoes[pos];

            /* Salva estado do escopo anterior */
            int var_backup_total = total_variaveis;

            /* Avalia argumentos antes de criar variáveis da função */
            Valor args_vals[32];
            int total_args = 0;
            No *arg_no = no->argumentos;
            while (arg_no && total_args < 32) {
                args_vals[total_args++] = avaliar(arg_no);
                arg_no = arg_no->proximo;
            }

            /* Associa parâmetros */
            No *p = f->parametros;
            int idx = 0;
            while (p) {
                if (buscar_variavel(p->nome) != -1 && total_variaveis >= var_backup_total) {
                    /* Se já tem no escopo local da função */
                }
                variaveis[total_variaveis].nome = strdup(p->nome);
                variaveis[total_variaveis].tipo_declarado = p->tipo_dado;
                if (idx < total_args) {
                    variaveis[total_variaveis].val = copiar_valor(args_vals[idx]);
                } else {
                    variaveis[total_variaveis].val = valor_padrao(p->tipo_dado);
                }
                total_variaveis++;
                idx++;
                p = p->proximo;
            }

            /* Executa o corpo da função */
            interpretar_bloco(f->corpo);

            Valor retorno = valor_padrao(TIPO_RS);
            if (sinal_retorne) {
                retorno = copiar_valor(valor_retorno);
                sinal_retorne = 0;
            }

            /* Restaura escopo */
            total_variaveis = var_backup_total;
            return retorno;
        }

        case NO_LEITURA: {
            /* input em expressão */
            char buf[256];
            if (fgets(buf, sizeof(buf), stdin)) {
                buf[strcspn(buf, "\r\n")] = 0;
                char *end;
                double n = strtod(buf, &end);
                if (*end == '\0' && end != buf) {
                    v.tipo = TIPO_RS;
                    v.numero = n;
                } else {
                    v.tipo = TIPO_TEXTO_T;
                    v.texto = strdup(buf);
                }
            }
            return v;
        }

        default:
            return v;
    }
}

/* Interpreta uma sequência/bloco de nós */
static void interpretar_bloco(No *bloco) {
    for (No *cmd = bloco; cmd != NULL; cmd = cmd->proximo) {
        if (sinal_pare || sinal_retorne)
            return;

        if (cmd->tipo == NO_DECLARACAO) {
            if (buscar_variavel(cmd->nome) != -1)
                erro("Variável duplicada", cmd->nome);

            int pos = total_variaveis++;
            variaveis[pos].nome = strdup(cmd->nome);
            variaveis[pos].tipo_declarado = cmd->tipo_dado;
            if (cmd->expressao) {
                variaveis[pos].val = avaliar(cmd->expressao);
            } else {
                variaveis[pos].val = valor_padrao(cmd->tipo_dado);
            }
        }
        else if (cmd->tipo == NO_INICIALIZACAO) {
            if (buscar_variavel(cmd->nome) != -1)
                erro("Variável duplicada", cmd->nome);

            int pos = total_variaveis++;
            variaveis[pos].nome = strdup(cmd->nome);
            variaveis[pos].tipo_declarado = cmd->tipo_dado;
            variaveis[pos].val = valor_padrao(cmd->tipo_dado);
        }
        else if (cmd->tipo == NO_ATRIBUICAO) {
            int pos = buscar_variavel(cmd->nome);
            if (pos == -1)
                erro("Variável não encontrada", cmd->nome);
            variaveis[pos].val = avaliar(cmd->expressao);
        }
        else if (cmd->tipo == NO_ATRIB_TREM) {
            int pos = buscar_variavel(cmd->nome);
            if (pos == -1)
                erro("Variável não encontrada", cmd->nome);
            if (variaveis[pos].tipo_declarado != TIPO_TREM_T)
                erro("Variável não é do tipo trem", cmd->nome);
            Valor idx_val = avaliar(cmd->esquerda);
            int idx = (int)para_numero(idx_val);
            if (idx < 0 || idx >= MAX_TREM_TAM)
                erro("Índice de trem inválido", cmd->nome);
            Valor novo_val = avaliar(cmd->expressao);
            variaveis[pos].val.elementos[idx] = novo_val;
            if (idx >= variaveis[pos].val.trem_tamanho)
                variaveis[pos].val.trem_tamanho = idx + 1;
        }
        else if (cmd->tipo == NO_PRINT) {
            No *arg = cmd->argumentos;
            while (arg) {
                Valor v = avaliar(arg);
                imprimir_valor(v);
                arg = arg->proximo;
            }
            printf("\n");
        }
        else if (cmd->tipo == NO_LEITURA) {
            No *arg = cmd->argumentos;
            if (arg) {
                /* Se primeiro arg for texto, imprime */
                if (arg->tipo == NO_TEXTO) {
                    printf("%s", arg->texto);
                    arg = arg->proximo;
                }
                if (arg && (arg->tipo == NO_VARIAVEL || arg->tipo == NO_DECLARACAO)) {
                    int pos = buscar_variavel(arg->nome);
                    if (pos == -1)
                        erro("Variável não encontrada", arg->nome);
                    char buf[256];
                    if (fgets(buf, sizeof(buf), stdin)) {
                        buf[strcspn(buf, "\r\n")] = 0;
                        if (variaveis[pos].tipo_declarado == TIPO_RS) {
                            variaveis[pos].val.tipo = TIPO_RS;
                            variaveis[pos].val.numero = atof(buf);
                        } else {
                            variaveis[pos].val.tipo = TIPO_TEXTO_T;
                            variaveis[pos].val.texto = strdup(buf);
                        }
                    }
                }
            }
        }
        else if (cmd->tipo == NO_SE) {
            Valor cond = avaliar(cmd->expressao);
            if (para_bool(cond)) {
                interpretar_bloco(cmd->bloco_entao);
            } else if (cmd->bloco_senao) {
                interpretar_bloco(cmd->bloco_senao);
            }
        }
        else if (cmd->tipo == NO_ATE) {
            if (cmd->nome && cmd->inicio) {
                /* ate (i R$ 1 << 6) ent % ... % */
                int pos = buscar_variavel(cmd->nome);
                if (pos == -1) {
                    pos = total_variaveis++;
                    variaveis[pos].nome = strdup(cmd->nome);
                    variaveis[pos].tipo_declarado = cmd->tipo_dado;
                }
                variaveis[pos].val = avaliar(cmd->inicio);

                while (!sinal_pare && !sinal_retorne) {
                    Valor limite = avaliar(cmd->expressao);
                    if (!para_bool(limite)) break;
                    interpretar_bloco(cmd->corpo);
                    variaveis[pos].val.numero += 1.0;
                }
                if (sinal_pare) sinal_pare = 0;
            } else {
                while (!sinal_pare && !sinal_retorne) {
                    Valor cond = avaliar(cmd->expressao);
                    if (!para_bool(cond)) break;
                    interpretar_bloco(cmd->corpo);
                }
                if (sinal_pare) sinal_pare = 0;
            }
        }
        else if (cmd->tipo == NO_ENQUANTO) {
            while (!sinal_pare && !sinal_retorne) {
                Valor cond = avaliar(cmd->expressao);
                if (!para_bool(cond)) break;
                interpretar_bloco(cmd->corpo);
            }
            if (sinal_pare) sinal_pare = 0;
        }
        else if (cmd->tipo == NO_REPETE) {
            while (!sinal_pare && !sinal_retorne) {
                interpretar_bloco(cmd->corpo);
            }
            if (sinal_pare) sinal_pare = 0;
        }
        else if (cmd->tipo == NO_PARAR) {
            sinal_pare = 1;
            return;
        }
        else if (cmd->tipo == NO_RETORNE) {
            if (cmd->expressao) {
                valor_retorno = avaliar(cmd->expressao);
            } else {
                valor_retorno = valor_padrao(TIPO_RS);
            }
            sinal_retorne = 1;
            return;
        }
        else if (cmd->tipo == NO_FUNCAO) {
            /* Cadastra função */
            int pos = total_funcoes++;
            funcoes[pos].nome = strdup(cmd->nome);
            funcoes[pos].stopmeters = cmd->stopmeters;
            funcoes[pos].parametros = cmd->parametros;
            funcoes[pos].corpo = cmd->corpo;
        }
        else if (cmd->tipo == NO_CHAMADA) {
            avaliar(cmd);
        }
    }
}

/* Ponto de entrada do interpretador */
void interpretar(No *programa) {
    total_variaveis = 0;
    total_funcoes = 0;
    sinal_pare = 0;
    sinal_retorne = 0;

    interpretar_bloco(programa);
}
