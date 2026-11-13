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

## 2. Estrutura e Arquitetura do Projeto

O projeto segue estritamente a arquitetura padrão em C com **Flex** e **Bison**:

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

---

## 3. Como Compilar e Executar (via WSL / Linux)

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

---

## 4. Testes Iniciais

### Casos Válidos (`testes/validos/`)
- `01_aritmetica.xum`: Precedência de operadores aritméticos e expressões parentizadas.
- `02_condicional_logica.xum`: Desvios condicionais compostos com operadores relacionais e lógicos (`&`, `|`).
- `03_lacos.xum`: Laço condicional `enquanto` e repetição contínua `repete` com interrupção `pare`.
- `04_funcoes.xum`: Declaração de funções com parâmetros tipados e instrução `retorne`.
- `05_trem.xum`: Instanciação, acesso e modificação indexada de listas (`trem`).

### Casos Inválidos (`testes/invalidos/`)
- **Léxicos:**
  - `01_erro_lexico_simbolo.xum`: Símbolo não reconhecido (`@`).
  - `02_erro_lexico_texto.xum`: Literal de texto aberto com `#` sem fechamento antes da quebra de linha.
- **Sintáticos:**
  - `03_erro_sintatico_expressao.xum`: Expressão incompleta (`x R$ { 10 + }`).
  - `04_erro_sintatico_bloco.xum`: Bloco delimitador `%` não fechado.
- **Semânticos:**
  - `05_erro_semantico_var_inexistente.xum`: Uso de variável não declarada.
  - `06_erro_semantico_divisao_zero.xum`: Divisão aritmética por zero.
  - `07_erro_semantico_var_duplicada.xum`: Redeclaração de variável no mesmo escopo.
