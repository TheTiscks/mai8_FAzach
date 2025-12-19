/*
Функция subqueue создаёт новую очередь result, содержащую все элементы исходной очереди initial, кроме последних without элементов.
Очередь — односвязный список с указателями на голову и хвост.
Новый список полностью независим (новые узлы).
Реализация за один проход:

Сначала считаем общее количество элементов в initial.
Если without ≥ количества — возвращаем пустую очередь.
Копируем первые count - without элементов в result.
*/

#include <stdlib.h>

typedef int tvalue;

typedef struct queue_item {
    tvalue value;
    struct queue_item *next;
} queue_item;

typedef struct queue {
    queue_item *head;
    queue_item *tail;
} queue;

/**
 * Возвращает:
 *  0 — успех
 * -1 — один из параметров NULL
 * -2 — ошибка выделения памяти
 *
 * result->head и result->tail устанавливаются корректно
 * result независим от initial
 */
int subqueue(queue *initial, size_t without, queue **result) {
    if (!initial || !result) {
        return -1;
    }

    *result = malloc(sizeof(queue));
    if (!*result) {
        return -2;
    }
    (*result)->head = NULL;
    (*result)->tail = NULL;

    // Считаем количество элементов в initial
    size_t total = 0;
    queue_item *curr = initial->head;
    while (curr) {
        total++;
        curr = curr->next;
    }

    if (without >= total) {
        // Все элементы исключаем → пустая очередь
        return 0;
    }

    size_t to_copy = total - without;

    curr = initial->head;
    queue_item **tail_ptr = &(*result)->head;

    for (size_t i = 0; i < to_copy; i++) {
        queue_item *new_item = malloc(sizeof(queue_item));
        if (!new_item) {
            // Очистка уже построенной части
            queue_item *tmp = (*result)->head;
            while (tmp) {
                queue_item *next = tmp->next;
                free(tmp);
                tmp = next;
            }
            free(*result);
            *result = NULL;
            return -2;
        }
        new_item->value = curr->value;
        new_item->next = NULL;

        *tail_ptr = new_item;
        tail_ptr = &new_item->next;

        if (i == to_copy - 1) {
            (*result)->tail = new_item;
        }

        curr = curr->next;
    }

    return 0;
}

/* Пример использования */
#include <stdio.h>

void push_back(queue *q, tvalue val) {
    queue_item *n = malloc(sizeof(queue_item));
    n->value = val;
    n->next = NULL;
    if (!q->head) {
        q->head = q->tail = n;
    } else {
        q->tail->next = n;
        q->tail = n;
    }
}

void print_queue(queue *q) {
    queue_item *p = q->head;
    while (p) {
        printf("%d ", p->value);
        p = p->next;
    }
    printf("\n");
}

void clear_queue(queue *q) {
    queue_item *p = q->head;
    while (p) {
        queue_item *next = p->next;
        free(p);
        p = next;
    }
    q->head = q->tail = NULL;
}

int main() {
    queue src = { NULL, NULL };
    push_back(&src, 1);
    push_back(&src, 2);
    push_back(&src, 3);
    push_back(&src, 4);
    push_back(&src, 5);
    // src: 1 -> 2 -> 3 -> 4 -> 5

    queue *res = NULL;
    int code = subqueue(&src, 2, &res);  // без последних 2 (4 и 5)

    if (code == 0 && res) {
        printf("Результат (без последних 2): ");
        print_queue(res);  // 1 2 3

        clear_queue(res);
        free(res);
    } else {
        printf("Ошибка: %d\n", code);
    }

    clear_queue(&src);
    return 0;
}