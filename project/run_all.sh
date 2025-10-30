#!/bin/bash
SOURCE="sort_compare_detailed.c"
EXEC="./sort_compare_detailed"
INPUT_SCRIPT="./gerar_input.sh"

echo "========================================================"
echo "          INICIANDO PROCESSO DE BENCHMARK"
echo "========================================================"

echo "🔄 Gerando novos dados de entrada..."
if [ -f "$INPUT_SCRIPT" ]; then
  bash "$INPUT_SCRIPT"
else
  echo "⚠️  Script $INPUT_SCRIPT não encontrado. Pulei esta etapa."
fi
echo "✅ Dados gerados!"
echo ""

if grep -q $'\r' "$SOURCE"; then
  echo "🧹 Corrigindo quebras de linha estilo Windows..."
  dos2unix "$SOURCE"
fi

echo "🔧 Compilando programa ($SOURCE)..."
gcc "$SOURCE" -o "$EXEC" -O2 -Wall
if [ $? -ne 0 ]; then
  echo "❌ Erro na compilação!"
  exit 1
fi
echo "✅ Compilação concluída! ($EXEC)"
echo ""

run_test() {
  local input=$1
  echo "🚀 Rodando benchmark com: $input ..."
  echo "----------------------------------------------------------------------------------------"
  $EXEC "$input"
  echo "📊 Execução concluída para $input"
  echo ""
}

for input in inputs/input1.txt inputs/input2.txt inputs/input3.txt; do
  if [ -f "$input" ]; then
    run_test "$input"
  else
    echo "⚠️  Arquivo $input não encontrado — pulando."
  fi
done

echo "========================================================"
echo "🏁 Execuções concluídas!"
echo "========================================================"