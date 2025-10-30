#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Métricas separadas por tipo
typedef struct {
    unsigned long long comparisons; // todas comparações (if, <=, <, > usadas para decidir)
    unsigned long long swaps;       // quantas trocas/swaps foram realizadas
    unsigned long long copies;      // atribuições/cópias (cópia para aux, atribuição direta)
    double cpu_seconds;
} Metrics;

int* clone_array(const int* src, size_t n) {
    int* dst = (int*)malloc(n * sizeof(int));
    if (!dst) { fprintf(stderr,"Erro de memória\n"); exit(EXIT_FAILURE); }
    memcpy(dst, src, n * sizeof(int));
    return dst;
}

int* read_input_file(const char* filename, size_t* out_n) {
    FILE* f = fopen(filename, "r");
    if (!f) { fprintf(stderr, "Não foi possível abrir '%s'\n", filename); exit(EXIT_FAILURE); }
    size_t capacity = 1024, n = 0;
    int* arr = (int*)malloc(capacity * sizeof(int));
    if (!arr) { fprintf(stderr,"Erro malloc\n"); exit(EXIT_FAILURE); }
    int value;
    while (fscanf(f, "%d", &value) == 1) {
        if (n >= capacity) {
            capacity *= 2;
            arr = (int*)realloc(arr, capacity * sizeof(int));
            if (!arr) { fprintf(stderr,"Erro realloc\n"); exit(EXIT_FAILURE); }
        }
        arr[n++] = value;
    }
    fclose(f);
    *out_n = n;
    return arr;
}

/* ================= Selection Sort ================= */
void selection_sort(int* v, size_t n, Metrics* m) {
    clock_t start = clock();
    m->comparisons = m->swaps = m->copies = 0;
    for (size_t i = 0; i + 1 < n; ++i) {
        size_t min = i;
        for (size_t j = i + 1; j < n; ++j) {
            m->comparisons++;
            if (v[j] < v[min]) min = j;
        }
        if (min != i) {
            int tmp = v[i];
            v[i] = v[min];
            v[min] = tmp;
            m->swaps++;       // contamos a troca como 1 swap
            m->copies += 3;   // opcionalmente também contamos as 3 atribuições como cópias
        }
    }
    clock_t end = clock();
    m->cpu_seconds = (double)(end - start) / CLOCKS_PER_SEC;
}

/* ================= Merge Sort (recursivo) ================= */
void merge_internal(int* v, int left, int mid, int right, Metrics* m) {
    int n = right - left + 1;
    int* aux = (int*)malloc(n * sizeof(int));
    if (!aux) { fprintf(stderr,"Erro malloc merge\n"); exit(EXIT_FAILURE); }
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        m->comparisons++;
        if (v[i] <= v[j]) {
            aux[k++] = v[i++];
            m->copies++;
        } else {
            aux[k++] = v[j++];
            m->copies++;
        }
    }
    while (i <= mid) { aux[k++] = v[i++]; m->copies++; }
    while (j <= right) { aux[k++] = v[j++]; m->copies++; }
    for (k = 0; k < n; ++k) { v[left + k] = aux[k]; m->copies++; }
    free(aux);
}

void merge_sort_rec(int* v, int left, int right, Metrics* m) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_rec(v, left, mid, m);
        merge_sort_rec(v, mid + 1, right, m);
        merge_internal(v, left, mid, right, m);
    }
}

void merge_sort(int* v, size_t n, Metrics* m) {
    clock_t start = clock();
    m->comparisons = m->swaps = m->copies = 0;
    merge_sort_rec(v, 0, (int)n - 1, m);
    clock_t end = clock();
    m->cpu_seconds = (double)(end - start) / CLOCKS_PER_SEC;
}

/* ================= Heap Sort (0-based) ================= */
void heapify(int* v, int n, int i, Metrics* m) {
    while (1) {
        int largest = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        if (l < n) {
            m->comparisons++;
            if (v[l] > v[largest]) largest = l;
        }
        if (r < n) {
            m->comparisons++;
            if (v[r] > v[largest]) largest = r;
        }
        if (largest != i) {
            int tmp = v[i];
            v[i] = v[largest];
            v[largest] = tmp;
            m->swaps++;
            m->copies += 3;
            i = largest;
        } else break;
    }
}

void heap_sort(int* v, size_t n, Metrics* m) {
    clock_t start = clock();
    m->comparisons = m->swaps = m->copies = 0;
    for (int i = (int)(n/2) - 1; i >= 0; --i) heapify(v, (int)n, i, m);
    for (int i = (int)n - 1; i >= 1; --i) {
        int tmp = v[0]; v[0] = v[i]; v[i] = tmp;
        m->swaps++;
        m->copies += 3;
        heapify(v, i, 0, m);
    }
    clock_t end = clock();
    m->cpu_seconds = (double)(end - start) / CLOCKS_PER_SEC;
}

/* ================= util ================= */
void print_array(const int* v, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        printf("%d", v[i]);
        if (i + 1 < n) printf(" ");
    }
    printf("\n");
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]); return EXIT_FAILURE; }
    size_t n;
    int* original = read_input_file(argv[1], &n);
    if (n == 0) { fprintf(stderr,"Arquivo vazio\n"); free(original); return EXIT_FAILURE; }

    int* v_sel = clone_array(original, n);
    int* v_mer = clone_array(original, n);
    int* v_heap = clone_array(original, n);

    Metrics m_sel = {0}, m_mer = {0}, m_heap = {0};

    // executa
    selection_sort(v_sel, n, &m_sel);
    merge_sort(v_mer, n, &m_mer);
    heap_sort(v_heap, n, &m_heap);

    // imprime resumo na saída
    printf("n=%zu\n", n);
    printf("Selection: comp=%llu swaps=%llu copies=%llu cpu=%.6fs\n",
           m_sel.comparisons, m_sel.swaps, m_sel.copies, m_sel.cpu_seconds);
    printf("Merge:     comp=%llu swaps=%llu copies=%llu cpu=%.6fs\n",
           m_mer.comparisons, m_mer.swaps, m_mer.copies, m_mer.cpu_seconds);
    printf("Heap:      comp=%llu swaps=%llu copies=%llu cpu=%.6fs\n",
           m_heap.comparisons, m_heap.swaps, m_heap.copies, m_heap.cpu_seconds);

    // grava CSV (append). Header:
    // n,sel_comp,sel_swaps,sel_copies,sel_cpu,mer_comp,mer_swaps,mer_copies,mer_cpu,heap_comp,heap_swaps,heap_copies,heap_cpu,filename
    FILE* csv = fopen("results_compare_detailed.csv", "a");
    if (csv) {
        fprintf(csv, "%zu,%llu,%llu,%llu,%.6f,%llu,%llu,%llu,%.6f,%llu,%llu,%llu,%.6f,%s\n",
                n,
                m_sel.comparisons, m_sel.swaps, m_sel.copies, m_sel.cpu_seconds,
                m_mer.comparisons, m_mer.swaps, m_mer.copies, m_mer.cpu_seconds,
                m_heap.comparisons, m_heap.swaps, m_heap.copies, m_heap.cpu_seconds,
                argv[1]);
        fclose(csv);
    } else {
        fprintf(stderr,"Não foi possível abrir results_compare_detailed.csv para escrita\n");
    }

    free(original); free(v_sel); free(v_mer); free(v_heap);
    return EXIT_SUCCESS;
}