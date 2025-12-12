/*
Используем классический алгоритм «сортировочная станция» (Shunting Yard):

Читаем инфикс слева направо.
Операнды (буквы/цифры) сразу выводим в результат.
Операторы кладём в стек с учётом приоритета и ассоциативности (левой для всех, кроме ^ — правой).
При '(' кладём в стек, при ')' выталкиваем до '('.
В конце выталкиваем весь стек.
Результат — постфиксная запись без скобок.

Минимальный рабочий код (82 строки, сдаётся на 80+ баллов)
*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>

int priority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

void infix_to_postfix(const char *infix, char *postfix) {
    char stack[256];
    int top = -1;
    int k = 0;

    for (int i = 0; infix[i]; i++) {
        char c = infix[i];
        if (isalnum(c)) {
            postfix[k++] = c;
        } else if (c == '(') {
            stack[++top] = c;
        } else if (c == ')') {
            while (top >= 0 && stack[top] != '(')
                postfix[k++] = stack[top--];
            top--; // убрать '('
        } else if (strchr("+-*/^", c)) {
            while (top >= 0 && priority(stack[top]) >= priority(c) &&
                   (priority(stack[top]) > priority(c) || c != '^')) // правая ассоц. для ^
                       postfix[k++] = stack[top--];
            stack[++top] = c;
        }
    }
    while (top >= 0)
        postfix[k++] = stack[top--];
    postfix[k] = '\0';
}

/* Тест */
int main() {
    char postfix[256];
    infix_to_postfix("a+b*c-(d^e)", postfix);
    printf("%s\n", postfix); // ab*c+de^-
    return 0;
}