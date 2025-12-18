/*
Задача — сжатие последовательностей одинаковых лексем (слов из букв и цифр) в формате лексема(количество), если количество ≥ 2.
Лексема — непрерывная последовательность алфавитно-цифровых символов (isalnum(c)).
Разделители (пробел, таб, \n) остаются как есть и разделяют лексему.
Правила:

Если лексема встречается 1 раз → выводим её как есть.
Если ≥2 раза подряд → лексема(число).
Разделители между одинаковыми лексемами удаляются.
Пустых лексем нет.

Алгоритм:

Читаем входной файл посимвольно до EOF.
Накапливаем текущую лексему в буфер, считаем её длину и количество повторений.
Когда встречаем разделитель или конец файла:
Если предыдущая группа лексем повторялась ≥2 раза → выводим лексема(кол-во).
Иначе выводим каждую лексему по отдельности (с разделителями).
Выводим текущий разделитель (если не конец файла).

Обрабатываем символ конца файла корректно
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define BUF_SIZE 1024

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");
    if (!in || !out) {
        perror("fopen");
        return 1;
    }

    char buf[BUF_SIZE];          // текущая лексема
    int len = 0;                 // длина текущей лексемы
    int count = 0;               // сколько раз подряд встретилась
    int c;

    // Предыдущая лексема (для сравнения)
    char prev_buf[BUF_SIZE];
    int prev_len = 0;

    while ((c = fgetc(in)) != EOF) {
        if (isalnum(c)) {
            if (len < BUF_SIZE - 1)
                buf[len++] = c;
        } else {
            // Встретили разделитель — обрабатываем накопленную лексему
            if (len > 0) {
                if (count == 0) {
                    // Первая лексема в группе
                    memcpy(prev_buf, buf, len);
                    prev_len = len;
                    count = 1;
                } else if (len == prev_len && memcmp(buf, prev_buf, len) == 0) {
                    // Та же лексема — увеличиваем счётчик
                    count++;
                } else {
                    // Другая лексема — выводим предыдущую группу
                    if (count >= 2)
                        fprintf(out, "%.*s(%d)", prev_len, prev_buf, count);
                    else
                        for (int i = 0; i < count; i++)
                            fprintf(out, "%.*s", prev_len, prev_buf);

                    // Начинаем новую группу
                    memcpy(prev_buf, buf, len);
                    prev_len = len;
                    count = 1;
                }
                len = 0;  // сбрасываем буфер текущей
            }
            // Выводим разделитель
            fputc(c, out);
        }
    }

    // Обработка последней группы после EOF
    if (len > 0 || count > 0) {
        if (count == 0 && len > 0) {
            // Единственная лексема в конце файла
            fprintf(out, "%.*s", len, buf);
        } else if (len > 0) {
            // Новая лексема в конце
            if (count >= 2)
                fprintf(out, "%.*s(%d)", prev_len, prev_buf, count);
            else
                for (int i = 0; i < count; i++)
                    fprintf(out, "%.*s", prev_len, prev_buf);
            fprintf(out, "%.*s", len, buf);
        } else {
            // Только предыдущая группа
            if (count >= 2)
                fprintf(out, "%.*s(%d)", prev_len, prev_buf, count);
            else
                for (int i = 0; i < count; i++)
                    fprintf(out, "%.*s", prev_len, prev_buf);
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}