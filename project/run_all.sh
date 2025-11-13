#!/bin/bash
SOURCE="sort_compare_detailed.c"
EXEC="./sort_compare_detailed"

echo "========================================================"
echo "           INICIANDO PROCESSO DE BENCHMARK"
echo "========================================================"

if grep -q $'\r' "$SOURCE"; then
  echo "🧹 Corrigindo quebras de linha estilo Windows..."
  dos2unix "$SOURCE"
fi

echo "🔧 Compilando programa ($SOURCE)..."
gcc "$SOURCE" -o "$EXEC" -O2 -Wall -lm
if [ $? -ne 0 ]; then
  echo "❌ Erro na compilação!"
  exit 1
fi
echo "✅ Compilação concluída! ($EXEC)"
echo ""

echo "🧹 Preparando o diretório 'resultado'..."
mkdir -p resultado
rm -f resultado/*
mkdir -p resultado/heap
mkdir -p resultado/merge
mkdir -p resultado/selection
echo "✅ Diretório 'resultado' e subpastas estão prontos (e vazios)."
echo ""

run_test() {
  local input=$1
  echo "🚀 Rodando benchmark com: $input ..."
  echo "----------------------------------------------------------------------------------------"
  $EXEC "$input"
  echo "📊 Execução concluída para $input"
  echo ""
}

for input in inputs/dtaleat100kdup0.txt inputs/dtaleat100kuni1.txt inputs/dtconcv100kdup2.txt; do
  if [ -f "$input" ]; then
    run_test "$input"
  else
    echo "⚠️  Arquivo $input não encontrado — pulando."
  fi
done

echo "========================================================"
echo "🏁 Execuções concluídas!"
echo "========================================================"