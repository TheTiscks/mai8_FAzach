/*
ИДЕЯ РЕШЕНИЯ

Считываем весь файл построчно (матрица может быть «рваной» — строки разной длины).
Определяем количество строк (rows) и максимальную длину строки (cols).
Выделяем динамический двумерный массив int** matrix размером rows × cols.
Заполняем его значениями из файла, а все пропущенные позиции (короткие строки) — нулями.
Транспонируем: выводим по столбцам исходной матрицы (т.е. i и j меняем местами).
Освобождаем память.


Два прохода по файлу: первый — только для определения размеров (rows и max_cols).
Пропущенные элементы автоматически становятся нулями благодаря calloc.
Транспонирование делается просто перестановкой индексов при выводе — не нужно создавать вторую матрицу.
Поддерживает произвольные пробельные разделители (пробелы, табы).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char line[1024];
    int rows = 0;
    int max_cols = 0;
    int *col_counts = NULL;          // количество элементов в каждой строке

    // Первый проход: определяем размеры
    while (fgets(line, sizeof(line), f)) {
        rows++;
        int cols = 0;
        char *p = line;
        int val;
        while (sscanf(p, "%d", &val) == 1) {
            cols++;
            while (*p && *p != ' ' && *p != '\t' && *p != '\n') p++;
            while (*p == ' ' || *p == '\t') p++;
        }
        if (cols > max_cols) max_cols = cols;

        col_counts = realloc(col_counts, rows * sizeof(int));
        col_counts[rows-1] = cols;
    }
    rewind(f);

    if (rows == 0) {
        fclose(f);
        return 0;
    }

    // Выделяем матрицу rows × max_cols
    int **matrix = malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++)
        matrix[i] = calloc(max_cols, sizeof(int));

    // Второй проход: заполняем
    int r = 0;
    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        int c = 0;
        int val;
        while (sscanf(p, "%d", &val) == 1) {
            if (c < max_cols) matrix[r][c++] = val;
            while (*p && *p != ' ' && *p != '\t' && *p != '\n') p++;
            while (*p == ' ' || *p == '\t') p++;
        }
        r++;
    }
    fclose(f);

    // Вывод транспонированной матрицы (max_cols строк, rows столбцов)
    for (int j = 0; j < max_cols; j++) {
        for (int i = 0; i < rows; i++) {
            if (i > 0) putchar(' ');
            printf("%d", matrix[i][j]);
        }
        putchar('\n');
    }

    // Освобождение памяти
    for (int i = 0; i < rows; i++) free(matrix[i]);
    free(matrix);
    free(col_counts);

    return 0;
}