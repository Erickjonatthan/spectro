#!/usr/bin/env bash

# Script para compilar e executar todos os testes da linguagem Spectro.xum

set -e

echo "============================================="
echo "   Compilando o interpretador Spectro.xum   "
echo "============================================="
make clean
make

echo ""
echo "============================================="
echo "        Executando Programas Exemplos        "
echo "============================================="
for prog in programa1.xum programa2.xum programa3.xum programa4.xum; do
    echo "--- Executando: $prog ---"
    ./mini "$prog"
    echo ""
done

echo "============================================="
echo "          Executando Casos Válidos           "
echo "============================================="
for t in testes/validos/*.xum; do
    echo "--- Teste válido: $t ---"
    ./mini "$t"
    echo ""
done

echo "============================================="
echo "         Executando Casos Inválidos          "
echo "  (Todos devem falhar com erro adequado)     "
echo "============================================="
set +e
for t in testes/invalidos/*.xum; do
    echo "--- Teste inválido: $t ---"
    ./mini "$t" 2>&1
    echo ""
done

echo "============================================="
echo "     Bateria de testes finalizada com sucesso! "
echo "============================================="
