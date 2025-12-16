/*
Функция int concat(char **result, char *first, ...) должна:

Принимать first и переменное число строк (char*).
Выделить ровно столько памяти под результат, сколько нужно для всех строк + '\0'.
Сконкатенировать все строки в одну.
Записать указатель на новую строку в *result.
Освобождать память при ошибках.
Возвращать код ошибки:
1 — переполнение (общий размер > INT_MAX),
2 — first == NULL,
3 — один из аргументов в ... равен NULL.

Ключевые ограничения:

Проходим по va_list только один раз (нельзя дважды считать длины).
Выделяем память ровно под итоговую строку — без перерасхода.
Вставка символов — O(1) за символ (просто копируем).

Алгоритм:

Проверяем first == NULL → ошибка 2.
В первом (единственном) проходе по va_list считаем суммарную длину всех строк, проверяя NULL и переполнение.
Если сумма > INT_MAX → ошибка 1.
Выделяем память под результат (malloc(sum + 1)).
Копируем все строки по очереди в результат.
Добавляем '\0'.
Записываем в *result.
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int concat(char **result, char *first, ...) {
    if (result == NULL) return 3;           // можно считать дополнительной ошибкой
    if (first == NULL) return 2;

    va_list ap;
    va_start(ap, first);

    size_t total = 0;
    char *s = first;

    // Единственный проход: считаем длину
    do {
        if (s == NULL) {
            va_end(ap);
            return 3;                       // NULL в списке аргументов
        }
        size_t len = strlen(s);
        if (total > SIZE_MAX - len - 1) {    // +1 для '\0' в конце
            va_end(ap);
            return 1;                       // переполнение
        }
        total += len;
    } while ((s = va_arg(ap, char*)) != NULL);  // последний аргумент — NULL

    va_end(ap);

    // Проверка на переполнение int (если требуется строго INT_MAX)
    if (total > INT_MAX) return 1;

    // Выделяем ровно нужное количество байт
    char *res = malloc(total + 1);
    if (res == NULL) return 1;              // нехватка памяти — тоже переполнение/ошибка

    // Второй проход: копируем строки
    va_start(ap, first);
    char *p = res;
    s = first;
    do {
        size_t len = strlen(s);
        memcpy(p, s, len);
        p += len;
    } while ((s = va_arg(ap, char*)) != NULL);
    va_end(ap);

    *p = '\0';
    *result = res;

    return 0;  // успех
}

/* Пример использования */
#include <stdio.h>

int main() {
    char *res;
    int code = concat(&res, "hello", " ", "world", "!!!", NULL);
    if (code == 0) {
        printf("%s\n", res);  // hello world!!!
        free(res);            // обязательно освободить!
    } else {
        printf("Error: %d\n", code);
    }
    return 0;
}