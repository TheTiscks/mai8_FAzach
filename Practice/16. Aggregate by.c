/*
Функция aggregate_by реализует свёртку (fold/reduce) двусвязного списка по предикату.

Проходим по списку в направлении direction:
direction == 0 — с конца к началу (от tail к head)
direction != 0 — с начала к концу (от head к tail)

Для каждого элемента, удовлетворяющего predicate(value) != 0, вызываем бинарную операцию fold(accumulator, value, &new_acc).
Начинаем с initial_accumulator.
Финальное значение аккумулятора записываем в *result.

Типы структур — стандартные для двусвязного списка.
*/

#include <stdlib.h>

// Тип значения элемента списка
typedef int tvalue;

// Элемент двусвязного списка
typedef struct bl_item {
    tvalue value;
    struct bl_item *next;
    struct bl_item *prev;
} bl_item;

// Двусвязный список
typedef struct bl {
    bl_item *head;
    bl_item *tail;
} bl;

/**
 * Возвращает:
 *  0 — успех
 * -1 — один из параметров NULL (from, predicate, fold, result)
 * -2 — список пустой
 */
int aggregate_by(bl *from,
                 int direction,
                 int (*predicate)(tvalue value),
                 tvalue initial_accumulator,
                 tvalue (*fold)(tvalue accumulator, tvalue from_list, tvalue *result)) {
    if (!from || !predicate || !fold || !result) {
        return -1;
    }

    if (!from->head) {  // пустой список
        return -2;
    }

    tvalue acc = initial_accumulator;
    bl_item *curr;

    if (direction == 0) {           // с конца к началу
        curr = from->tail;
        while (curr) {
            if (predicate(curr->value)) {
                tvalue new_acc;
                new_acc = fold(acc, curr->value, &new_acc);
                acc = new_acc;
            }
            curr = curr->prev;
        }
    } else {                        // с начала к концу
        curr = from->head;
        while (curr) {
            if (predicate(curr->value)) {
                tvalue new_acc;
                new_acc = fold(acc, curr->value, &new_acc);
                acc = new_acc;
            }
            curr = curr->next;
        }
    }

    *result = acc;
    return 0;
}

/* Пример использования */
#include <stdio.h>

int is_positive(tvalue v) { return v > 0; }

tvalue sum_fold(tvalue acc, tvalue val, tvalue *res) {
    *res = acc + val;
    return *res;  // возвращаемое значение не используется, главное — *res
}

int main() {
    bl list = { NULL, NULL };

    // Функция добавления в конец (для теста)
    void push_back(bl *l, tvalue v) {
        bl_item *n = malloc(sizeof(bl_item));
        n->value = v;
        n->next = NULL;
        n->prev = l->tail;
        if (l->tail) l->tail->next = n;
        else l->head = n;
        l->tail = n;
    }

    push_back(&list, -5);
    push_back(&list, 3);
    push_back(&list, 7);
    push_back(&list, -2);
    push_back(&list, 4);

    tvalue result;
    int code = aggregate_by(&list, 1, is_positive, 0, sum_fold, &result);

    if (code == 0) {
        printf("Сумма положительных: %d\n", result);  // 3 + 7 + 4 = 14
    } else {
        printf("Ошибка: %d\n", code);
    }

    return 0;
}