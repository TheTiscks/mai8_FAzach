/*
Считываем входной файл построчно, определяем rows и максимальное cols.
Выделяем динамическую матрицу int** a размером rows × cols, заполняем нулями (calloc), читаем значения (недостающие остаются 0).
Выводим размеры: rows cols.
Находим для каждой строки минимальный элемент (row_min[i]).
Находим для каждого столбца максимальный элемент (col_max[j]).
Проходим по матрице: если a[i][j] == row_min[i] && a[i][j] == col_max[j] — это седловая точка → записываем в выходной файл строку i j (индексы с 0).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) { perror("input"); return 1; }

    // Первый проход — определяем размеры
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

    // min в строках, max в столбцах
    int *row_min = malloc(rows * sizeof(int));
    int *col_max = malloc(cols * sizeof(int));

    for (int i = 0; i < rows; i++) {
        row_min[i] = a[i][0];
        for (int j = 1; j < cols; j++)
            if (a[i][j] < row_min[i]) row_min[i] = a[i][j];
    }
    for (int j = 0; j < cols; j++) {
        col_max[j] = a[j][0];
        for (int i = 1; i < rows; i++)
            if (a[i][j] > col_max[j]) col_max[j] = a[i][j];
    }

    // Записываем седловые точки в выходной файл
    FILE *out = fopen(argv[2], "w");
    if (!out) { perror("output"); return 1; }

    int found = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (a[i][j] == row_min[i] && a[i][j] == col_max[j]) {
                fprintf(out, "%d %d\n", i, j);
                found = 1;
            }
        }
    }
    if (!found) fprintf(out, "none\n");  // часто требуют

    fclose(out);

    // Освобождение
    for (int i = 0; i < rows; i++) free(a[i]);
    free(a);
    free(row_min);
    free(col_max);

    return 0;
}