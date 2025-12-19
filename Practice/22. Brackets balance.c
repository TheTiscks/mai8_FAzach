/*
Классическая проверка баланса скобок с помощью стека.

Валидируем массив setup (проверяем, что все открывающие и закрывающие скобки из разных пар не повторяются и соответствуют друг другу).
Проходим по строке string_to_check посимвольно.
Если символ — открывающая скобка из setup → кладём её в стек.
Если символ — закрывающая → проверяем, совпадает ли она с верхушкой стека (по конфигурации setup). Если да — выталкиваем, иначе — ошибка.
В конце стек должен быть пуст → баланс правильный.

Возвращаем через *result:

0 — правильный баланс
иначе — некорректный
*/

#include <stdlib.h>
#include <string.h>

typedef char tchar;  // значение в стеке — символ скобки

typedef struct stack_item {
    tchar value;
    struct stack_item *next;
} stack_item;

typedef struct stack {
    stack_item *top;
} stack;

typedef struct bracket_pair {
    tchar open;
    tchar close;
} bracket_pair;

/**
 * Возвращает:
 *  0 — правильный баланс скобок
 * -1 — один из параметров NULL
 * -2 — некорректная конфигурация setup (повторения или несоответствия)
 * -3 — в строке встречен неизвестный символ скобки
 * -4 — баланс нарушен (несоответствие или остаток в стеке)
 *
 * result — указатель на int, куда записывается 0 или код ошибки (>0)
 */
int brackets_balance(char *string_to_check, int *result,
                     bracket_pair *setup, size_t setup_pairs_count) {
    if (!string_to_check || !result || !setup || setup_pairs_count == 0) {
        if (result) *result = 1;
        return -1;
    }

    // Валидация setup: проверяем уникальность всех символов и парность
    for (size_t i = 0; i < setup_pairs_count; i++) {
        for (size_t j = i + 1; j < setup_pairs_count; j++) {
            if (setup[i].open == setup[j].open || setup[i].close == setup[j].close ||
                setup[i].open == setup[j].close || setup[i].close == setup[j].open) {
                *result = 2;
                return -2;
            }
        }
    }

    stack s = { NULL };

    const char *p = string_to_check;
    while (*p) {
        int found = 0;

        // Проверяем, открывающая ли скобка
        for (size_t i = 0; i < setup_pairs_count; i++) {
            if (*p == setup[i].open) {
                stack_item *n = malloc(sizeof(stack_item));
                if (!n) {
                    // Очистка стека при ошибке памяти
                    while (s.top) {
                        stack_item *tmp = s.top;
                        s.top = s.top->next;
                        free(tmp);
                    }
                    *result = 5;
                    return -5;
                }
                n->value = *p;
                n->next = s.top;
                s.top = n;
                found = 1;
                break;
            }
        }

        if (found) {
            p++;
            continue;
        }

        // Проверяем, закрывающая ли скобка
        for (size_t i = 0; i < setup_pairs_count; i++) {
            if (*p == setup[i].close) {
                if (!s.top || s.top->value != setup[i].open) {
                    // Очистка стека
                    while (s.top) {
                        stack_item *tmp = s.top;
                        s.top = s.top->next;
                        free(tmp);
                    }
                    *result = 4;
                    return -4;
                }
                stack_item *tmp = s.top;
                s.top = s.top->next;
                free(tmp);
                found = 1;
                break;
            }
        }

        if (!found) {
            // Неизвестный символ — пропускаем (по условию разрешены любые символы)
        }

        p++;
    }

    // В конце стек должен быть пуст
    if (s.top) {
        while (s.top) {
            stack_item *tmp = s.top;
            s.top = s.top->next;
            free(tmp);
        }
        *result = 4;
        return -4;
    }

    *result = 0;
    return 0;
}

/* Пример использования */
#include <stdio.h>

int main() {
    bracket_pair setup[] = {
        {'(', ')'},
        {'[', ']'},
        {'{', '}'}
    };
    size_t count = sizeof(setup) / sizeof(setup[0]);

    char str1[] = "({[]})";     // правильный
    char str2[] = "({[})";      // неправильный
    char str3[] = "a(b)c[d]e{f}g";  // правильный с посторонними символами

    int res;
    int code;

    code = brackets_balance(str1, &res, setup, count);
    printf("str1: code=%d, result=%d\n", code, res);  // 0, 0

    code = brackets_balance(str2, &res, setup, count);
    printf("str2: code=%d, result=%d\n", code, res);  // -4, 4

    code = brackets_balance(str3, &res, setup, count);
    printf("str3: code=%d, result=%d\n", code, res);  // 0, 0

    return 0;
}