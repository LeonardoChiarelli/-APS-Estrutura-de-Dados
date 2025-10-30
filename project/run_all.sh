#!/bin/bash

# --------------------------------------------------------
# Executa automaticamente todas as ordenações e gera CSV
# --------------------------------------------------------

SOURCE="sort_compare_detailed.c"
EXEC="./sort_compare_detailed"
OUTPUT_DIR="resultados"
INPUT_SCRIPT="./gerar_input.sh"

echo "🔄 Gerando novos dados de entrada..."
if [ -f "$INPUT_SCRIPT" ]; then
  bash "$INPUT_SCRIPT"
else
  echo "⚠️  Script $INPUT_SCRIPT não encontrado. Pulei esta etapa."
fi
echo "✅ Dados gerados!"

# Converte quebras de linha (evita erro $'\r')
if grep -q $'\r' "$SOURCE"; then
  echo "🧹 Corrigindo quebras de linha estilo Windows..."
  dos2unix "$SOURCE"
fi

# Compila o programa em C
echo "🔧 Compilando programa..."
gcc "$SOURCE" -o "$EXEC" -O2 -Wall
if [ $? -ne 0 ]; then
  echo "❌ Erro na compilação!"
  exit 1
fi
echo "✅ Compilação concluída!"

# Cria diretório de resultados
mkdir -p "$OUTPUT_DIR"

# Remove CSV antigo se existir
if [ -f results_compare_detailed.csv ]; then
  mv results_compare_detailed.csv "$OUTPUT_DIR/results_compare_detailed_$(date +%H%M%S).csv"
fi

# Função para executar cada input
run_test() {
  local input=$1
  echo "🚀 Rodando com $input ..."
  $EXEC "$input"
  echo "📊 Execução concluída para $input"
}

# Executa os 3 arquivos de input
for input in inputs/input1.txt inputs/input2.txt inputs/input3.txt; do
  if [ -f "$input" ]; then
    run_test "$input"
  else
    echo "⚠️  Arquivo $input não encontrado — pulando."
  fi
done

# Move CSV final para pasta de resultados
if [ -f results_compare_detailed.csv ]; then
  mv results_compare_detailed.csv "$OUTPUT_DIR/results_compare_detailed.csv"
  echo "✅ CSV final salvo em $OUTPUT_DIR/results_compare_detailed.csv"
else
  echo "⚠️ Nenhum CSV encontrado."
fi

echo "🏁 Execuções concluídas!"
