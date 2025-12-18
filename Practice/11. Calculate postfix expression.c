/*
Функция вычисляет значение арифметического выражения в постфиксной (обратной польской) записи.
Постфиксная нотация удобна тем, что не требует скобок и легко вычисляется с помощью стека.
Алгоритм:

Проходим по токенам строки postfix_expression (токены разделены пробелами/табами).
Если токен — число → кладём его в стек (как int).
Если токен — бинарный оператор (один символ):
Извлекаем два операнда из стека (правый, затем левый).
Ищем в переменном списке структур подходящую функцию-обработчик по строковому представлению оператора.
Вызываем найденную функцию: op_func(left, right, result).
Если она вернула 0 (успех) — кладём *result обратно в стек.
Иначе — возвращаем ненулевой код ошибки.

В конце в стеке должен остаться единственный элемент — результат вычисления → копируем его в *result и возвращаем 0.

Ошибки: деление на 0, недостаток операндов, неизвестный оператор, несколько результатов в стеке и т.д. — возвращаем код от функции-оператора или собственный.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Структура для описания бинарного оператора
typedef struct {
    const char *op_str;          // строковое представление, длина >=1
    int (*func)(int left, int right, int *result);
} Operator;

static int op_add(int l, int r, int *res) { *res = l + r; return 0; }
static int op_sub(int l, int r, int *res) { *res = l - r; return 0; }
static int op_mul(int l, int r, int *res) { *res = l * r; return 0; }
static int op_div(int l, int r, int *res) {
    if (r == 0) return -1;            // ошибка деления на 0
    *res = l / r;
    return 0;
}
// Можно добавить другие операторы

int calculate_postfix(char *postfix_expression, int *result, ...) {
    if (!postfix_expression || !result) return -1;

    va_list ap;
    va_start(ap, result);

    // Считываем все операторы до NULL
    Operator *ops = NULL;
    size_t ops_count = 0, ops_cap = 0;

    Operator *op;
    while ((op = va_arg(ap, Operator*)) != NULL) {
        if (!op->op_str || !op->func || strlen(op->op_str) == 0) {
            free(ops);
            va_end(ap);
            return -2;                // недопустимая структура
        }
        if (ops_count >= ops_cap) {
            ops_cap = ops_cap ? ops_cap * 2 : 8;
            ops = realloc(ops, ops_cap * sizeof(Operator));
            if (!ops) { va_end(ap); return -3; }
        }
        ops[ops_count++] = *op;
    }
    va_end(ap);

    int stack[1024];
    int top = -1;

    char *p = postfix_expression;
    while (*p) {
        while (isspace(*p)) p++;      // пропускаем разделители
        if (*p == '\0') break;

        if (isdigit(*p) || (*p == '-' && isdigit(*(p+1)))) {
            // Число
            int val = strtol(p, &p, 10);
            if (top >= 1023) { free(ops); return -4; }
            stack[++top] = val;
            continue;
        }

        // Оператор — ищем среди переданных
        int found = 0;
        for (size_t i = 0; i < ops_count; i++) {
            size_t len = strlen(ops[i].op_str);
            if (strncmp(p, ops[i].op_str, len) == 0) {
                if (top < 1) { free(ops); return -5; } // мало операндов
                int right = stack[top--];
                int left  = stack[top--];
                int res_val;
                int code = ops[i].func(left, right, &res_val);
                if (code != 0) { free(ops); return code; }
                if (top >= 1023) { free(ops); return -4; }
                stack[++top] = res_val;
                p += len;
                found = 1;
                break;
            }
        }
        if (!found) { free(ops); return -6; } // неизвестный оператор
    }

    free(ops);
    if (top != 0) return -7;              // лишние элементы в стеке
    *result = stack[0];
    return 0;                             // успех
}

/* Пример использования */
int main() {
    Operator add = {"+", op_add};
    Operator sub = {"-", op_sub};
    Operator mul = {"*", op_mul};
    Operator div = {"/", op_div};

    char expr[] = "5 3 2 * + 10 /";   // (5 + (3*2)) / 10 = 11/10 = 1
    int res;

    int code = calculate_postfix(expr, &res, &add, &sub, &mul, &div, NULL);
    if (code == 0)
        printf("Result: %d\n", res);  // 1
    else
        printf("Error code: %d\n", code);

    return 0;
}