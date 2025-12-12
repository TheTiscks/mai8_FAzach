'''
"На вход даны 2 пути к файлам, в 1 файле могут быть цифры, символы, табуляции, переносы строк, и нужно строки(например fklwwwww) из входного файла до EOF перевести в fklw(5) в выходной файл."
'''

/*
ИДЕЯ РЕШЕНИЯ (Run-Length Encoding для буквенных последовательностей)
Нужно сжать подряд идущие одинаковые буквы в формате буква(количество), но только если количество ≥ 2.
Если символ не буква или идёт одиночка — выводим его как есть.
Символы-разделители (цифры, пробелы, табы, переводы строк и т.д.) оставляем без изменений.
Алгоритм:

Открываем входной файл на чтение, выходной — на запись.
Читаем по одному символу до EOF.
Считаем длину текущей последовательности одинаковых букв.
Если длина ≥ 2 — выводим c(n), иначе — просто символ(ы).
Повторяем до конца файла.
*/

#include <stdio.h>
#include <ctype.h>

void rle_letters(const char *in_path, const char *out_path) {
    FILE *in  = fopen(in_path,  "r");
    FILE *out = fopen(out_path, "w");
    if (!in || !out) return;
    int prev = fgetc(in);
    if (prev == EOF) {
        fclose(in); fclose(out);
        return;
    }
    int count = 1;
    int c;
    while ((c = fgetc(in)) != EOF) {
        if (c == prev && isalpha(c)) {
            count++;
        } else {
            // выводим предыдущую группу
            if (void)(isalpha(prev) && count >= 2)
                ? fprintf(out, "%c(%d)", prev, count)
                : (count == 1 ? fputc(prev, out) : 0); // если count>1 и не буква — выводим все
            if (!isalpha(prev) || count == 1) {
                // если предыдущий не буква или одиночка — просто вывели один символ
                // а если была группа не-букв — нужно вывести остальные count-1
                for (int i = 1; i < count; i++) fputc(prev, out);
            }
            prev = c;
            count = 1;
        }
    }
    // последняя группа
    if (isalpha(prev) && count >= 2)
        fprintf(out, "%c(%d)", prev, count);
    else {
        for (int i = 0; i < count; i++) fputc(prev, out);
    }
    fclose(in);
    fclose(out);
}

int main() {
    rle_letters("input.txt", "output.txt");
    return 0;
}