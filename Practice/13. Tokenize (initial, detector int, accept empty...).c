/*
Функция tokenize — классический линейный разбор строки (лексер) за один проход.

Проходим по строке initial посимвольно.
Накапливаем текущую лексему в динамическом буфере (амортизированный O(1) на символ).
Для каждого нового символа c вызываем detector(c):
если возвращает 0 → c является разделителем → завершаем текущую лексему (если она не пустая).
иначе → c входит в лексему → добавляем его в буфер.

Если accept_empty_lexems == 0, то пустые лексемы (последовательности разделителей) игнорируем.
Все непустые (или все, если accept_empty_lexems != 0) лексемы сохраняем в динамический массив lexems (указатели на строки в выделенной памяти).
В *lexems_count записываем количество найденных лексем.
Обрабатываем все ошибки (NULL-параметры, malloc/realloc).
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int tokenize(char *initial,
             int (*detector)(int),
             int accept_empty_lexems,
             char ***lexems,
             size_t *lexems_count) {
    // Проверки параметров
    if (!initial || !detector || !lexems || !lexems_count) return -1;
    if (!accept_empty_lexems && accept_empty_lexems != 0) return -1; // на всякий случай

    *lexems = NULL;
    *lexems_count = 0;
    size_t capacity = 0;

    char *buf = NULL;          // текущая лексема
    size_t buf_len = 0;
    size_t buf_cap = 0;

    const char *p = initial;
    int prev_was_delim = 1;    // начало строки считаем как после разделителя

    while (*p) {
        int is_delim = (detector((unsigned char)*p) == 0);

        if (is_delim) {
            // Завершаем текущую лексему, если она есть
            if (buf_len > 0 || (accept_empty_lexems && !prev_was_delim)) {
                // Добавляем лексему в результат
                if (*lexems_count >= capacity) {
                    capacity = capacity ? capacity * 2 : 8;
                    char **tmp = realloc(*lexems, capacity * sizeof(char*));
                    if (!tmp) goto mem_error;
                    *lexems = tmp;
                }

                char *lex = malloc(buf_len + 1);
                if (!lex) goto mem_error;
                memcpy(lex, buf, buf_len);
                lex[buf_len] = '\0';

                (*lexems)[(*lexems_count)++] = lex;
            }

            buf_len = 0;
            prev_was_delim = 1;
        } else {
            // Добавляем символ в текущую лексему
            if (buf_len >= buf_cap) {
                buf_cap = buf_cap ? buf_cap * 2 : 64;
                char *tmp = realloc(buf, buf_cap);
                if (!tmp) goto mem_error;
                buf = tmp;
            }
            buf[buf_len++] = *p;
            prev_was_delim = 0;
        }
        p++;
    }

    // Последняя лексема после конца строки
    if (buf_len > 0 || (accept_empty_lexems && !prev_was_delim)) {
        if (*lexems_count >= capacity) {
            capacity = capacity ? capacity * 2 : 8;
            char **tmp = realloc(*lexems, capacity * sizeof(char*));
            if (!tmp) goto mem_error;
            *lexems = tmp;
        }

        char *lex = malloc(buf_len + 1);
        if (!lex) goto mem_error;
        memcpy(lex, buf, buf_len);
        lex[buf_len] = '\0';

        (*lexems)[(*lexems_count)++] = lex;
    }

    free(buf);
    return 0;

mem_error:
    // Освобождаем всё при ошибке памяти
    for (size_t i = 0; i < *lexems_count; i++)
        free((*lexems)[i]);
    free(*lexems);
    *lexems = NULL;
    *lexems_count = 0;
    free(buf);
    return 5;
}

/* Пример использования */
#include <ctype.h>
#include <stdio.h>

int is_separator(int c) {
    return isspace(c) || c == ',' || c == ';';
}

int main() {
    char ***lexems = NULL;
    size_t count = 0;

    char str[] = "hello, world; test   example";

    int code = tokenize(str, is_separator, 0, lexems, &count);

    switch (code) {
        case 0:
            printf("Найдено лексем: %zu\n", count);
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", (*lexems)[i]);
                free((*lexems)[i]);
            }
            free(*lexems);
            break;
        case 1: printf("NULL-параметр\n"); break;
        case 5: printf("Ошибка выделения памяти\n"); break;
    }
    return 0;
}