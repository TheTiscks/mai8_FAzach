/*
Классический алгоритм «Сортировочная станция» (Shunting Yard) Эдсгера Дейкстры с поддержкой переменного списка операторов и их приоритетов.

Читаем инфиксное выражение посимвольно, выделяем токены (числа и операторы).
Числа сразу копируем в выходную строку (с пробелом-разделителем).
Операторы:
Кладём в стек с учётом приоритета.
Если текущий оператор имеет меньший или равный приоритет — выталкиваем из стека в выход, пока это так.
При '(' — просто в стек.
При ')' — выталкиваем всё до '('.

В конце выталкиваем остаток стека.
Операторы ищем по строковому представлению в переданном va_list (валидируем структуры).
Обрабатываем ошибки: несбалансированные скобки, неизвестный оператор, некорректные структуры.

Выходная строка размещена в динамической памяти, токены разделены одним пробелом.
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// Структура для бинарного оператора
typedef struct {
    const char *op_str;     // строковое представление (не пустое, без цифр)
    int priority;           // приоритет (целое > 0)
} Operator;

int convert_to_postfix(char *infix_expression, char **postfix_expression, ...) {
    if (!infix_expression || !postfix_expression) return -1;

    *postfix_expression = NULL;

    va_list ap;
    va_start(ap, postfix_expression);

    Operator *ops = NULL;
    size_t ops_count = 0, ops_cap = 0;

    Operator *op;
    while ((op = va_arg(ap, Operator*)) != NULL) {
        if (!op->op_str || strlen(op->op_str) == 0 || op->priority <= 0) {
            free(ops);
            va_end(ap);
            return -2;  // недопустимая структура
        }
        // простая валидация: не содержит цифр
        for (const char *s = op->op_str; *s; s++) {
            if (isdigit(*s)) {
                free(ops);
                va_end(ap);
                return -2;
            }
        }
        if (ops_count >= ops_cap) {
            ops_cap = ops_cap ? ops_cap * 2 : 8;
            ops = realloc(ops, ops_cap * sizeof(Operator));
            if (!ops) { va_end(ap); return -3; }
        }
        ops[ops_count++] = *op;
    }
    va_end(ap);

    // Динамический буфер для результата
    char *out = NULL;
    size_t out_len = 0, out_cap = 0;

    char stack[256];          // стек операторов (строковые, до 10 символов каждый)
    int top = -1;

    const char *p = infix_expression;
    while (*p) {
        while (isspace(*p)) p++;

        if (isdigit(*p) || (*p == '-' && isdigit(*(p+1)))) {
            // Число — копируем в выход
            while (isdigit(*p) || *p == '-') {
                if (out_len >= out_cap) {
                    out_cap = out_cap ? out_cap * 2 : 128;
                    out = realloc(out, out_cap);
                    if (!out) { free(ops); return -3; }
                }
                out[out_len++] = *p++;
            }
            out[out_len++] = ' ';  // разделитель
            continue;
        }

        if (*p == '(') {
            stack[++top] = '(';
            p++;
            continue;
        }
        if (*p == ')') {
            while (top >= 0 && stack[top] != '(') {
                // выводим оператор из стека
                out_len += snprintf(out ? out + out_len : NULL,
                                    out_cap - out_len, "%c ", stack[top--]);
                if (out_len >= out_cap) {
                    out_cap *= 2;
                    out = realloc(out, out_cap);
                    if (!out) { free(ops); return -3; }
                }
            }
            if (top < 0) { free(ops); free(out); return -4; } // нет открывающей скобки
            top--; // убираем '('
            p++;
            continue;
        }

        // Оператор — ищем среди переданных
        int found = 0;
        for (size_t i = 0; i < ops_count; i++) {
            size_t len = strlen(ops[i].op_str);
            if (strncmp(p, ops[i].op_str, len) == 0 && !isalnum(*(p + len))) {
                // выталкиваем операторы с большим или равным приоритетом
                while (top >= 0 && stack[top] != '(') {
                    // находим приоритет текущего в стеке (упрощённо — сравниваем)
                    int stack_pri = 0;
                    for (size_t j = 0; j < ops_count; j++) {
                        if (stack[top] == ops[j].op_str[0]) { // упрощённо для 1-симв.
                            stack_pri = ops[j].priority;
                            break;
                        }
                    }
                    if (stack_pri < ops[i].priority) break;
                    out_len += snprintf(out ? out + out_len : NULL, out_cap - out_len,
                                        "%c ", stack[top--]);
                    if (out_len >= out_cap) {
                        out_cap *= 2;
                        out = realloc(out, out_cap);
                        if (!out) { free(ops); return -3; }
                    }
                }
                stack[++top] = ops[i].op_str[0];  // кладём первый символ (для простоты)
                p += len;
                found = 1;
                break;
            }
        }
        if (!found && !isspace(*p)) {
            free(ops); free(out);
            return -5;  // неизвестный символ
        }
        if (!found) p++;
    }

    // Остаток стека
    while (top >= 0) {
        if (stack[top] == '(') { free(ops); free(out); return -4; }
        out_len += snprintf(out ? out + out_len : NULL, out_cap - out_len,
                            "%c ", stack[top--]);
        if (out_len >= out_cap) {
            out_cap *= 2;
            out = realloc(out, out_cap);
            if (!out) { free(ops); return -3; }
        }
    }

    if (out_len > 0) out[out_len - 1] = '\0';  // убираем последний пробел
    else out = realloc(out, 1), out[0] = '\0';

    free(ops);
    *postfix_expression = out;
    return 0;
}

/* Пример */
int main() {
    Operator add = {"+", 1};
    Operator sub = {"-", 1};
    Operator mul = {"*", 2};
    Operator div = {"/", 2};
    Operator pow = {"^", 3};

    char *post;
    int code = convert_to_postfix(" (a + b) * c ^ d - e ", &post,
                                  &add, &sub, &mul, &div, &pow, NULL);

    if (code == 0) {
        printf("%s\n", post);  // a b + c d ^ * e -
        free(post);
    } else {
        printf("Error: %d\n", code);
    }
    return 0;
}