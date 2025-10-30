REPEATS=5
BIN="./sort_compare_detailed"
INPUT_DIR="./inputs"
OUT_CSV="results_compare_detailed.csv"

if [ ! -x "$BIN" ]; then
  echo "Erro: binário $BIN não encontrado ou não executável. Compile antes."
  exit 1
fi

if [ ! -d "$INPUT_DIR" ]; then
  echo "Erro: pasta $INPUT_DIR não encontrada. Crie e coloque seus arquivos .txt lá."
  exit 1
fi

# header CSV
echo "n,sel_comp,sel_swaps,sel_copies,sel_cpu,mer_comp,mer_swaps,mer_copies,mer_cpu,heap_comp,heap_swaps,heap_copies,heap_cpu,filename" > "$OUT_CSV"

for file in "$INPUT_DIR"/*.txt; do
  [ -e "$file" ] || continue
  echo "=== Testando: $file ==="
  for i in $(seq 1 $REPEATS); do
    echo "Run $i / $REPEATS"
    $BIN "$file"
  done
done

echo "Execuções concluídas. CSV: $OUT_CSV"
echo "Gerando relatório (aggregate_and_report.py)..."
python3 aggregate_and_report.py "$OUT_CSV"
echo "Relatório gerado: report.pdf (veja saída do script Python)"
