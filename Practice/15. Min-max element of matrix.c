/*
Задача похожа на предыдущую седловую точку, но с двумя разными типами элементов:

Элемент, который находится в одной строке с максимальным по модулю элементом матрицы (т.е. в строке, где |a[k][l]| — максимум во всей матрице).
Элемент, который находится в одном столбце с минимальным элементом матрицы (т.е. в столбце, где a[p][q] — минимум во всей матрице).

Нужно вывести в выходной файл индексы (i j, с 0) всех элементов матрицы, удовлетворяющих хотя бы одному из этих условий.
Алгоритм:

Два прохода по файлу: определяем размеры rows × max_cols.
Выделяем и заполняем матрицу (недостающие = 0).
Выводим rows cols в stdout.
Находим:
Максимум по модулю во всей матрице → индекс строки max_abs_row.
Минимум во всей матрице → индекс столбца min_col.

Проходим по матрице: если i == max_abs_rowилиj == min_col → записываем i j в выходной файл.
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>   // для fabs, но можно без неё (используем a < 0 ? -a : a)

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) { perror("input"); return 1; }

    // Первый проход — размеры
    char line[8192];
    int rows = 0, max_cols = 0;
    while (fgets(line, sizeof(line), in)) {
        rows++;
        int cols = 0;
        char *p = line;
        int val;
        while (sscanf(p, "%d", &val) == 1) {
            cols++;
            while (*p && !isspace(*p)) p++;
            while (isspace(*p)) p++;
        }
        if (cols > max_cols) max_cols = cols;
    }
    if (rows == 0) { fclose(in); return 0; }
    rewind(in);

    int cols = max_cols;

    // Выделяем и заполняем матрицу
    int **a = malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) a[i] = calloc(cols, sizeof(int));

    int r = 0;
    while (fgets(line, sizeof(line), in)) {
        char *p = line;
        int c = 0, val;
        while (sscanf(p, "%d", &val) == 1) {
            if (c < cols) a[r][c++] = val;
            while (*p && !isspace(*p)) p++;
            while (isspace(*p)) p++;
        }
        r++;
    }
    fclose(in);

    // Выводим размеры
    printf("%d %d\n", rows, cols);

    // Находим строку с максимумом по модулю
    int max_abs_val = 0;
    int max_abs_row = 0;
    // Находим минимум и его столбец
    int min_val = a[0][0];
    int min_col = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int val = a[i][j];
            int abs_val = val < 0 ? -val : val;

            if (i == 0 && j == 0) {
                max_abs_val = abs_val;
                min_val = val;
                continue;
            }

            if (abs_val > max_abs_val) {
                max_abs_val = abs_val;
                max_abs_row = i;
            }
            if (val < min_val) {
                min_val = val;
                min_col = j;
            }
        }
    }

    // Записываем в выходной файл все подходящие элементы
    FILE *out = fopen(argv[2], "w");
    if (!out) { perror("output"); return 1; }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i == max_abs_row || j == min_col) {
                fprintf(out, "%d %d\n", i, j);
            }
        }
    }

    fclose(out);

    // Освобождение
    for (int i = 0; i < rows; i++) free(a[i]);
    free(a);

    return 0;
}