# Spectro.xum

Linguagem de programação imperativa criada para a disciplina de Compiladores (UFRPE).

Equipe:
- Douglas G P Bezerra
- Erick J M Santos
- Matheus H H H C Gomes

---

## 1. Visão Geral da Linguagem

A **Spectro.xum** foi projetada com foco em sintaxe intuitiva, utilizando termos em português e delimitadores próprios.
- Declarações e atribuições usam chaves `{ }`
- Blocos de código são delimitados por `% ... %`
- Comandos terminam ao fim de linha
- Modalidade de execução: **Interpretador**

### Tipos de Dados

| Tipo | Descrição | Sintaxe de Exemplo |
|---|---|---|
| `R$` | Número (inteiro ou ponto flutuante) | `idade R$ {22}` |
| `bool` | Booleano (`vdd` / `fls`) | `ok bool {vdd}` |
| `txt` | Texto (delimitado por `#`) | `nome txt {#iae#}` |
| `abc` | Caractere individual | `letra abc {'a'}` |
| `trem` | Lista heterogênea | `lista trem {[1, 2, 3]}` |

### Operadores

- **Aritméticos:** `+`, `-`, `*`, `/`
- **Relacionais:** `==` (igual), `=>` (maior ou igual), `<=` (menor ou igual), `>>` (maior que), `<<` (menor que)
- **Lógicos:** `&` (e), `|` (ou), `!` (não)

### Comandos de Controle e I/O

- **Condicionais:** `se (cond) ent % ... %` com `senao % ... %` opcional
- **Laços:**
  - `ate (i R$ 1 << 6) ent % ... %` (laço com contador automático)
  - `enquanto (cond) ent % ... %` (laço condicional)
  - `repete % ... %` (laço interrompido com `pare`)
- **Entrada e Saída:**
  - `ss(...)` ou `screenshot(...)` para exibição na tela
  - `input(...)` para entrada de dados
- **Funções:**
  - `funsao nome(param tipo) % ... %`
  - Retorno de valor com `retorne <expressao>`

---

## 2. Especificação formal da linguagem

### Convenções léxicas

- Identificadores começam com uma letra e podem continuar com letras, números ou `_`.
- Números são inteiros ou decimais com ponto, por exemplo `12` e `3.14`.
- Textos são delimitados por `#`, não podem conter `#` nem quebra de linha.
- Um caractere é delimitado por aspas simples, por exemplo `'a'`.
- `?` inicia um comentário até o fim da linha; `?? ... ??` delimita comentário de bloco.
- Espaços e tabulações são ignorados. A quebra de linha separa comandos simples.

### Gramática (EBNF)

Os nomes entre aspas representam tokens literais; `IDENT`, `NUM`, `TEXTO` e
`CARACTERE` representam as categorias léxicas descritas acima. Colchetes
indicam elementos opcionais e chaves indicam repetição. `quebra` representa
o token de fim de linha. As regras de precedência ao final resolvem expressões
binárias sem parênteses.

```ebnf
programa           = { quebra }, { instrucao } ;
instrucao          = comando_simples, quebras
                   | comando_composto, [ quebras ] ;
quebras            = quebra, { quebra } ;
quebra             = fim_de_linha ;

comando_simples    = declaracao | atribuicao | impressao | comando_leitura
                   | retorno | interrupcao | chamada ;
declaracao         = IDENT, tipo, [ "{", expressao, "}" ] ;
atribuicao         = IDENT, "{", expressao, "}"
                   | IDENT, "[", expressao, "]", "{", expressao, "}" ;
impressao          = ("ss" | "screenshot"), "(", [ lista_expressoes ], ")" ;
comando_leitura    = "input", "(", [ lista_expressoes ], ")" ;
retorno            = "retorne", [ expressao ] ;
interrupcao        = "pare" ;
chamada            = IDENT, "(", [ lista_expressoes ], ")" ;

comando_composto   = condicional | laco_ate | laco_enquanto
                   | laco_repete | funcao ;
condicional        = "se", "(", expressao, ")", "ent", bloco,
                     [ quebras ], [ "senao", (bloco | condicional),
                     [ quebras ] ] ;
laco_ate           = "ate", "(", IDENT, tipo, expressao, "<<", expressao,
                     ")", "ent", bloco
                   | "ate", "(", IDENT, tipo, expressao, ")", "ent", bloco
                   | "ate", "(", expressao, ")", "ent", bloco ;
laco_enquanto      = "enquanto", "(", expressao, ")", [ "ent" ], bloco ;
laco_repete        = "repete", bloco ;
funcao             = "funsao", IDENT, [ "stopmeters" ], "(",
                     [ lista_parametros ], ")", bloco ;
bloco              = "%", [ quebras ], { instrucao }, "%" ;
lista_parametros   = parametro, { ",", parametro } ;
parametro          = IDENT, tipo ;
lista_expressoes   = expressao, { ",", expressao } ;

tipo               = "R$" | "bool" | "txt" | "abc" | "trem" ;
expressao          = NUM | TEXTO | CARACTERE | ("vdd" | "fls") | IDENT
                   | IDENT, "[", expressao, "]"
                   | chamada
                   | "input", [ "(", [ lista_expressoes ], ")" ]
                   | "[", [ lista_expressoes ], "]"
                   | "(", expressao, ")"
                   | "-", expressao
                   | "!", expressao
                   | expressao, operador, expressao ;
operador           = "+" | "-" | "*" | "/" | "==" | "=>"
                   | "<=" | ">>" | "<<" | "&" | "|" ;
```

Precedência, da menor para a maior, conforme a implementação: `|`, `&`, `!`,
operadores relacionais, `+` e `-`, `*` e `/`, e menos unário. Os operadores
binários são associativos à esquerda; `!` e o menos unário são associativos à
direita. A gramática de expressões acima é complementada por essa tabela de
precedência, como na especificação Bison.

## 3. Arquitetura e decisões de implementação

O projeto segue estritamente a arquitetura padrão em C com **Flex** e **Bison**:

1. **Análise léxica (`lexico.l`)**: o Flex reconhece palavras reservadas,
   identificadores, literais, operadores e delimitadores e os entrega como
   tokens.
2. **Análise sintática (`sintatico.y`)**: o Bison valida a sequência de tokens,
   aplica precedência de operadores e constrói a árvore sintática abstrata
   (AST).
3. **Representação (`arvore.h` e `arvore.c`)**: a AST representa expressões e
   comandos em nós, separando a estrutura do programa de sua execução.
4. **Execução (`interpretador.c`)**: o interpretador percorre a AST, avalia
   expressões, mantém os valores das variáveis e executa condicionais, laços e
   funções.
5. **Entrada do programa (`main.c`)**: abre o arquivo informado, inicia o
   parser e, se solicitado com `--arvore` (ou `-a`), imprime a AST antes de
   executar o programa.

Essa organização foi escolhida para manter as etapas do compilador separadas:
o parser constrói uma representação intermediária e o interpretador executa
essa representação, sem gerar código de máquina. Erros léxicos e sintáticos
são informados durante a análise; erros como variável inexistente, declaração
duplicada e divisão por zero são detectados durante a interpretação.

O interpretador usa tabelas de símbolos em memória e limites fixos para
variáveis, funções e elementos de `trem`, definidos em `interpretador.c`. É
uma implementação didática, não um runtime de produção.

### Arquivos principais

```
spectro/
├── lexico.l              # Análise léxica (Flex)
├── sintatico.y           # Análise sintática e construção da AST (Bison)
├── arvore.h              # Definição das estruturas e nós da AST
├── arvore.c              # Implementação de nós e visualizador da AST
├── interpretador.c       # Motor de execução / interpretação da AST
├── main.c                # Ponto de entrada, abertura de arquivos e opções
├── Makefile              # Regras de compilação
├── executar_testes.sh    # Script de automação de testes
├── programa1.xum         # Exemplo 1: Operações e expressões aritméticas
├── programa2.xum         # Exemplo 2: Condicionais e tomada de decisão
├── programa3.xum         # Exemplo 3: Repetição com laço ate
├── programa4.xum         # Exemplo 4: Declaração e chamada de função
└── testes/
    ├── validos/          # Casos de teste válidos cobrindo a linguagem
    └── invalidos/        # Casos com erros léxicos, sintáticos e semânticos
```

## 4. Como compilar e executar (Bash, Linux ou WSL)

### Pré-requisitos
- `gcc`
- `flex`
- `bison`
- `make`

### Compilação
```bash
make clean
make
```
O executável gerado se chamará `./mini`.

### Executando um Programa
```bash
./mini programa1.xum
```

Também é possível executar os exemplos de teste:
```bash
./mini testes/validos/01_aritmetica.xum
```

### Visualizando a AST
Para visualizar a Árvore Sintática Abstrata montada durante a análise:
```bash
./mini --arvore programa1.xum
```

### Executar Toda a Bateria de Testes
```bash
chmod +x executar_testes.sh
./executar_testes.sh
```

As instruções de compilação e execução acima são para um terminal Bash com
`gcc`, Flex, Bison e Make disponíveis. O script recompila o interpretador,
executa os exemplos e percorre os casos válidos e inválidos.

## 5. Testes e resultados esperados

As expectativas ficam em comentários no próprio arquivo `.xum`, usando
diretivas reconhecidas por `executar_testes.sh`. Por exemplo:

```text
? EXPECT-EXIT: 0
? EXPECT-STDOUT: Resultado: 18
```

Use uma diretiva `EXPECT-STDOUT` para cada linha esperada. Para casos inválidos,
informe `EXPECT-EXIT: 1` e os fragmentos esperados com
`EXPECT-STDOUT-CONTAINS` ou `EXPECT-STDERR-CONTAINS`, conforme o fluxo onde o
diagnóstico aparece. Os comentários continuam sendo ignorados pelo
interpretador.

### Casos válidos (`testes/validos/`)

| Arquivo | Cobertura | Saída esperada |
|---|---|---|
| `01_aritmetica.xum` | Precedência e parênteses | `Resultado expr: 28` |
| `02_condicional_logica.xum` | Condicional, operadores relacionais e lógicos | `Acesso liberado` |
| `03_lacos.xum` | `enquanto`, `repete` e `pare` | `Enquanto: 1`, `Enquanto: 2`, `Enquanto: 3`, `Repete: 10`, `Repete: 9`, `Repete: 8` (uma linha por mensagem) |
| `04_funcoes.xum` | Função com parâmetros e retorno | `Total soma3: 60` |
| `05_trem.xum` | Lista heterogênea, acesso e alteração | `Lista original: [10, 20, 30]`, `Lista modificada: [10, 99, 30]`, `Item 0: 10`, `Item 1: 99` (uma linha por mensagem) |

### Exemplos da raiz

| Arquivo | Saída esperada |
|---|---|
| `programa1.xum` | `Resultado: 18` |
| `programa2.xum` | `Maior de idade` |
| `programa3.xum` | `Soma de 1 a 5: 15` |
| `programa4.xum` | `14` |

### Casos inválidos (`testes/invalidos/`)

Todos devem encerrar com código de saída `1` e apresentar o diagnóstico
esperado no fluxo indicado nas diretivas do teste.

| Arquivo | Resultado esperado |
|---|---|
| `01_erro_lexico_simbolo.xum` | Erro léxico: caractere `@` desconhecido |
| `02_erro_lexico_texto.xum` | Erro léxico: texto não fechado com `#` |
| `03_erro_sintatico_expressao.xum` | Erro sintático: expressão incompleta, com `}` inesperado |
| `04_erro_sintatico_bloco.xum` | Erro sintático: fim do arquivo antes do fechamento do bloco `%` |
| `05_erro_semantico_var_inexistente.xum` | `ERRO --> Variável não encontrada {variavel_fantasma}` |
| `06_erro_semantico_divisao_zero.xum` | `ERRO --> Operação inválida {divisão por zero}` |
| `07_erro_semantico_var_duplicada.xum` | `ERRO --> Variável duplicada {total}` |

`executar_testes.sh` recompila o interpretador e executa os quatro exemplos da
raiz, os cinco casos válidos e os sete casos inválidos. Nos casos válidos,
compara exatamente stdout e stderr com as linhas anotadas. Nos inválidos,
compara o código de saída e verifica que stdout ou stderr contenha todos os
fragmentos de diagnóstico esperados; isso evita depender do texto detalhado
que pode variar entre versões do Bison. O script apresenta o total de casos
aprovados/reprovados e termina com código diferente de zero se algum teste
falhar.
