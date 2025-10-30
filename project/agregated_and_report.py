#!/usr/bin/env python3
# aggregate_and_report.py
# Uso: python3 aggregate_and_report.py results_compare_detailed.csv
# Gera: aggregated_results.csv, plots/*.png, report.pdf

import sys
import os
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

if len(sys.argv) < 2:
    print("Usage: python3 aggregate_and_report.py results_compare_detailed.csv")
    sys.exit(1)

csv_file = sys.argv[1]
df = pd.read_csv(csv_file)

# extrair colunas e normalizar nomes
# colunas esperadas: n,sel_comp,sel_swaps,sel_copies,sel_cpu,...
# o arquivo salva o filename na última coluna
# grupo por filename (cada arquivo de entrada)
grouped = df.groupby('filename')

agg_rows = []
plots_dir = "plots"
os.makedirs(plots_dir, exist_ok=True)

for fname, group in grouped:
    # calcular médias e desvios
    mean = group.mean(numeric_only=True)
    std = group.std(numeric_only=True)
    n = int(group['n'].iloc[0])

    row = {
        'filename': fname,
        'n': n,
        'sel_comp_mean': mean['sel_comp'],
        'sel_comp_std': std['sel_comp'],
        'sel_swaps_mean': mean['sel_swaps'],
        'sel_copies_mean': mean['sel_copies'],
        'sel_cpu_mean': mean['sel_cpu'],
        'mer_comp_mean': mean['mer_comp'],
        'mer_swaps_mean': mean['mer_swaps'],
        'mer_copies_mean': mean['mer_copies'],
        'mer_cpu_mean': mean['mer_cpu'],
        'heap_comp_mean': mean['heap_comp'],
        'heap_swaps_mean': mean['heap_swaps'],
        'heap_copies_mean': mean['heap_copies'],
        'heap_cpu_mean': mean['heap_cpu'],
    }
    agg_rows.append(row)

    # Criar gráfico comparativo: custo composto = comparisons + copies + swaps*penalty
    # Aqui mostramos 3 gráficos: comparisons, copies, swaps (um por tipo) + CPU
    algs = ['Selection', 'Merge', 'Heap']
    comps = [mean['sel_comp'], mean['mer_comp'], mean['heap_comp']]
    copies = [mean['sel_copies'], mean['mer_copies'], mean['heap_copies']]
    swaps = [mean['sel_swaps'], mean['mer_swaps'], mean['heap_swaps']]
    cpus = [mean['sel_cpu'], mean['mer_cpu'], mean['heap_cpu']]

    x = range(len(algs))

    plt.figure(figsize=(8,5))
    plt.bar(x, comps)
    plt.xticks(x, algs)
    plt.ylabel("Average Comparisons")
    plt.title(f"Comparisons - {os.path.basename(fname)} (n={n})")
    plt.tight_layout()
    p1 = os.path.join(plots_dir, f"comparisons_{os.path.basename(fname)}.png")
    plt.savefig(p1)
    plt.close()

    plt.figure(figsize=(8,5))
    plt.bar(x, copies)
    plt.xticks(x, algs)
    plt.ylabel("Average Copies")
    plt.title(f"Copies - {os.path.basename(fname)} (n={n})")
    plt.tight_layout()
    p2 = os.path.join(plots_dir, f"copies_{os.path.basename(fname)}.png")
    plt.savefig(p2)
    plt.close()

    plt.figure(figsize=(8,5))
    plt.bar(x, swaps)
    plt.xticks(x, algs)
    plt.ylabel("Average Swaps")
    plt.title(f"Swaps - {os.path.basename(fname)} (n={n})")
    plt.tight_layout()
    p3 = os.path.join(plots_dir, f"swaps_{os.path.basename(fname)}.png")
    plt.savefig(p3)
    plt.close()

    plt.figure(figsize=(8,5))
    plt.bar(x, cpus)
    plt.xticks(x, algs)
    plt.ylabel("Average CPU seconds")
    plt.title(f"CPU time - {os.path.basename(fname)} (n={n})")
    plt.tight_layout()
    p4 = os.path.join(plots_dir, f"cpu_{os.path.basename(fname)}.png")
    plt.savefig(p4)
    plt.close()

# aggregated CSV
agg_df = pd.DataFrame(agg_rows)
agg_df.to_csv("aggregated_results.csv", index=False)
print("Aggregated CSV: aggregated_results.csv")
print("Plots: directory 'plots/'")

# Gerar relatório PDF com os gráficos
with PdfPages('report.pdf') as pdf:
    # página de capa / sumário
    plt.figure(figsize=(11,8.5))
    plt.axis('off')
    plt.title("Comparativo de Algoritmos de Ordenação\nSelectionSort | MergeSort | HeapSort", fontsize=18, pad=20)
    summary_lines = [
        f"Arquivo de origem: {csv_file}",
        f"Entradas testadas: {len(grouped)}",
        f"Execuções por entrada (amostras): {len(df) // len(grouped) if len(grouped)>0 else 0}",
        "",
        "Métricas calculadas (médias): comparisons, swaps, copies, cpu_seconds",
        "",
        "Gerado automaticamente por aggregate_and_report.py"
    ]
    y = 0.8
    for l in summary_lines:
        plt.text(0.05, y, l, fontsize=12, va='top')
        y -= 0.06
    pdf.savefig()
    plt.close()

    # inserir um conjunto de páginas com gráficos por arquivo
    for fname, group in grouped:
        base = os.path.basename(fname)
        for kind in ['comparisons', 'copies', 'swaps', 'cpu']:
            plotfile = os.path.join("plots", f"{kind}_{base}.png")
            if os.path.exists(plotfile):
                img = plt.imread(plotfile)
                plt.figure(figsize=(11,8.5))
                plt.imshow(img)
                plt.axis('off')
                plt.title(f"{kind.capitalize()} - {base}", fontsize=14)
                pdf.savefig()
                plt.close()

    # página final com notas
    plt.figure(figsize=(11,8.5))
    plt.axis('off')
    notes = [
        "Notas:",
        "- 'comparisons' conta todas as comparações usadas para decisão de ordenação.",
        "- 'swaps' conta quantas trocas/swaps foram realizadas (um swap é contado como 1).",
        "- 'copies' conta atribuições/cópias (ex.: cópias para vetor auxiliar no merge e atribuições para o array).",
        "- Ajuste as convenções conforme necessidade (ex.: contar swaps como 3 cópias ou 1).",
        "",
        "Para reproduzir: rode run_all.sh após compilar o programa C e colocar os arquivos em inputs/."
    ]
    y = 0.9
    for l in notes:
        plt.text(0.05, y, l, fontsize=11, va='top')
        y -= 0.06
    pdf.savefig()
    plt.close()

print("Relatório PDF gerado: report.pdf")