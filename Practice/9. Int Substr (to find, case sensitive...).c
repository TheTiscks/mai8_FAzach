/*
Функция ищет подстроку to_find в переменном числе строк (список заканчивается NULL).

Проходим по всем строкам из ... (va_list).
Для каждой строки используем strstr (с учётом case_sensitive).
Если подстрока найдена — сохраняем указатель на исходную строку в массив results.
Массив выделяем динамически (malloc → realloc при необходимости).
В *results_count записываем количество найденных строк.
Ошибки:
1 — to_find == NULL
2 — results == NULL
3 — results_count == NULL
4 — ошибка выделения памяти
При успехе возвращаем 0 и не освобождаем входные строки (они принадлежат вызывающему).
При ошибке 4 — освобождаем уже выделенный массив results.
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int substr(char *to_find, int case_sensitive, char ***results, size_t *results_count, ...) {
    // Проверка обязательных параметров
    if (to_find == NULL) return 1;
    if (results == NULL) return 2;
    if (results_count == NULL) return 3;

    *results = NULL;
    *results_count = 0;
    size_t capacity = 0;

    va_list ap;
    va_start(ap, results_count);

    char *str;
    while ((str = va_arg(ap, char*)) != NULL) {
        if (str == NULL) continue; // пропускаем NULL-строки, но не ошибку

        const char *haystack = str;
        const char *needle = to_find;

        // Если нечувствительно к регистру — ищем вручную
        if (!case_sensitive) {
            size_t nlen = strlen(needle);
            if (nlen == 0) goto found;

            for (; *haystack; haystack++) {
                if (tolower((unsigned char)*haystack) == tolower((unsigned char)*needle)) {
                    const char *h = haystack + 1;
                    const char *n = needle + 1;
                    for (; *n; h++, n++) {
                        if (tolower((unsigned char)*h) != tolower((unsigned char)*n))
                            break;
                    }
                    if (*n == '\0') goto found;
                }
            }
            continue; // не найдено
        } else {
            if (strstr(str, to_find) == NULL)
                continue;
        }

    found:
        // Добавляем строку в результат
        if (*results_count >= capacity) {
            capacity = capacity ? capacity * 2 : 4;
            char **tmp = realloc(*results, capacity * sizeof(char*));
            if (!tmp) {
                free(*results);
                *results = NULL;
                *results_count = 0;
                va_end(ap);
                return 4;
            }
            *results = tmp;
        }
        (*results)[*results_count] = str;
        (*results_count)++;
    }

    va_end(ap);
    return 0;
}

/* Пример использования с обработкой кодов */
#include <stdio.h>

int main() {
    char *s1 = "Hello World";
    char *s2 = "hello there";
    char *s3 = "HELLO again";
    char *s4 = "no match";

    char **res = NULL;
    size_t cnt = 0;

    int code = substr("hello", 0, &res, &cnt, s1, s2, s3, s4, NULL);

    switch (code) {
        case 0:
            printf("Найдено: %zu\n", cnt);
            for (size_t i = 0; i < cnt; i++)
                printf("%s\n", res[i]);
            free(res);  // обязательно освободить массив указателей
            break;
        case 1: printf("Ошибка: to_find == NULL\n"); break;
        case 2: printf("Ошибка: results == NULL\n"); break;
        case 3: printf("Ошибка: results_count == NULL\n"); break;
        case 4: printf("Ошибка: нехватка памяти\n"); break;
    }

    return 0;
}