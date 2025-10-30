#!/bin/bash

# Quantidade de números
QTD=100000

# Limite dos números
MIN=1
MAX=1000000

echo "Gerando arquivos de entrada com $QTD números cada..."

# Gera input1.txt
shuf -i $MIN-$MAX -n $QTD > inputs/input1.txt
echo "input1.txt gerado."

# Gera input2.txt
shuf -i $MIN-$MAX -n $QTD > inputs/input2.txt
echo "input2.txt gerado."

# Gera input3.txt
shuf -i $MIN-$MAX -n $QTD > inputs/input3.txt
echo "input3.txt gerado."

echo "✅ Todos os arquivos foram gerados com sucesso!"