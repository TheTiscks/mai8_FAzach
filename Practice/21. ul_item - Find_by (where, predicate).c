/*
Функция find_by создаёт новый односвязный список result, содержащий копии всех элементов из where, для которых предикат возвращает ненулевое значение.
Новый список полностью независим от исходного (выделяем новые узлы).
Обработка ошибок: NULL-параметры и нехватка памяти.
Реализация:

Проверяем параметры.
Инициализируем result->head = NULL.
Проходим по исходному списку.
Для подходящих элементов выделяем новый узел, копируем значение, добавляем в конец результирующего списка (через указатель на tail).
Возвращаем количество найденных или код ошибки.
*/

#include <stdlib.h>

typedef int tvalue;

typedef struct ul_item {
    tvalue value;
    struct ul_item *next;
} ul_item;

typedef struct ul {
    ul_item *head;
} ul;

/**
 * Возвращает:
 * >=0 — количество найденных элементов
 * -1 — ошибка выделения памяти
 * -2 — один из параметров NULL (where, predicate, result)
 *
 * result->head должен быть NULL перед вызовом (гарантируем внутри)
 */
int find_by(ul *where, int (*predicate)(tvalue value), ul *result) {
    if (!where || !predicate || !result) {
        return -2;
    }

    result->head = NULL;
    ul_item **tail = &result->head;  // указатель на последний next
    int count = 0;

    ul_item *curr = where->head;
    while (curr != NULL) {
        if (predicate(curr->value)) {
            ul_item *new_item = malloc(sizeof(ul_item));
            if (new_item == NULL) {
                // Очистка уже построенного результата
                ul_item *tmp = result->head;
                while (tmp != NULL) {
                    ul_item *next = tmp->next;
                    free(tmp);
                    tmp = next;
                }
                result->head = NULL;
                return -1;
            }
            new_item->value = curr->value;
            new_item->next = NULL;

            *tail = new_item;
            tail = &new_item->next;
            count++;
        }
        curr = curr->next;
    }

    return count;
}

/* Пример использования */
#include <stdio.h>

int is_positive(tvalue v) { return v > 0; }

void push_front(ul *list, tvalue val) {
    ul_item *n = malloc(sizeof(ul_item));
    n->value = val;
    n->next = list->head;
    list->head = n;
}

void print_list(ul *list) {
    for (ul_item *p = list->head; p; p = p->next) {
        printf("%d ", p->value);
    }
    printf("\n");
}

void clear_list(ul *list) {
    ul_item *p = list->head;
    while (p) {
        ul_item *next = p->next;
        free(p);
        p = next;
    }
    list->head = NULL;
}

int main() {
    ul source = { NULL };
    push_front(&source, -3);
    push_front(&source, 8);
    push_front(&source, -1);
    push_front(&source, 4);
    push_front(&source, 0);
    // source: 0 -> 4 -> -1 -> 8 -> -3

    ul positive = { NULL };
    int cnt = find_by(&source, is_positive, &positive);

    if (cnt >= 0) {
        printf("Найдено положительных: %d\n", cnt);
        print_list(&positive);  // 4 8
    } else {
        printf("Ошибка: %d\n", cnt);
    }

    clear_list(&source);
    clear_list(&positive);

    return 0;
}