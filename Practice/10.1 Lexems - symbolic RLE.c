/*
Задача — Run-Length Encoding (RLE) на уровне символов: сжимаем последовательности подряд идущих одинаковых символов (букв или цифр) в формат символ(количество), если количество ≥ 2.
Пробелы, табуляции и переводы строк не сжимаются и выводятся как есть (они разрывают последовательности).
Алгоритм:

Читаем входной файл посимвольно (fgetc).
Ведём счётчик текущего символа и его повторений.
Когда символ меняется или встречается разделитель (пробел/\t/\n):
Выводим предыдущую группу: если count ≥ 2 → c(count), иначе просто c count раз.
Начинаем новую группу с текущего символа.

Разделители выводим сразу, без сжатия.
После EOF обрабатываем последнюю группу.
*/

#include <stdio.h>
#include <ctype.h>

int is_delim(int c) {
    return c == ' ' || c == '\t' || c == '\n';
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
        return 1;
    }

    FILE *in  = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");
    if (!in || !out) {
        perror("fopen");
        return 1;
    }

    int prev = fgetc(in);
    if (prev == EOF) {
        fclose(in); fclose(out);
        return 0;
    }

    int count = 1;
    int c;

    while ((c = fgetc(in)) != EOF) {
        if (is_delim(prev)) {
            // Разделители всегда выводим как есть
            fputc(prev, out);
            prev = c;
            count = 1;
            continue;
        }

        if (c == prev) {
            count++;
        } else {
            // Выводим группу предыдущего символа
            if (count >= 2)
                fprintf(out, "%c(%d)", prev, count);
            else
                for (int i = 0; i < count; i++)
                    fputc(prev, out);

            prev = c;
            count = 1;
        }
    }

    // Последняя группа после EOF
    if (!is_delim(prev)) {
        if (count >= 2)
            fprintf(out, "%c(%d)", prev, count);
        else
            for (int i = 0; i < count; i++)
                fputc(prev, out);
    } else {
        fputc(prev, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}