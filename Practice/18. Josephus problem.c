/*
Задача Иосифа Флавия на кольцевом списке: удаляем каждый step-й элемент, пока не останется один.
Решение:

Снимаем глубокую копию списка (новые узлы, оригинал не трогаем).
В зависимости от direction:
direction != 0 — идём вперёд (next)
direction == 0 — идём назад (prev)

Начинаем с первого элемента (head для прямого, tail для обратного).
В цикле:
Проходим step-1 шагов в выбранном направлении.
Удаляем текущий узел (связываем prev и next).
Повторяем, пока элементов > 1.

В конце в списке остаётся один узел → записываем его индекс (вычисляем по порядку от оригинала) и значение.

Для вычисления индекса проходим по оригинальному списку и сравниваем значения (предполагаем уникальность, как обычно в задаче).
*/

#include <stdlib.h>

typedef int tvalue;

typedef struct cl_item {
    tvalue value;
    struct cl_item *next;
    struct cl_item *prev;
} cl_item;

typedef struct cl {
    cl_item *head;
    cl_item *tail;
} cl;

// Глубокая копия кольцевого списка
static cl* copy_list(const cl *src) {
    if (!src || !src->head) return NULL;

    cl *copy = malloc(sizeof(cl));
    copy->head = NULL;
    copy->tail = NULL;

    cl_item *orig = src->head;
    do {
        cl_item *n = malloc(sizeof(cl_item));
        n->value = orig->value;
        if (!copy->head) {
            copy->head = copy->tail = n;
            n->next = n->prev = n;
        } else {
            n->next = copy->head;
            n->prev = copy->tail;
            copy->tail->next = n;
            copy->head->prev = n;
            copy->tail = n;
        }
        orig = orig->next;
    } while (orig != src->head);

    return copy;
}

int Josephus_problem(cl *from, int direction, int step,
                     int *result_id, tvalue *result_value) {
    if (!from || !from->head || step <= 0 || !result_id || !result_value) {
        return -1;
    }
    if (!from->head->next) {  // один элемент
        *result_id = 0;
        *result_value = from->head->value;
        return 0;
    }

    cl *work = copy_list(from);
    if (!work) return -2;

    cl_item *curr = direction ? work->head : work->tail;

    while (work->head != work->head->next) {  // пока >1 элемента
        for (int i = 1; i < step; i++) {
            curr = direction ? curr->next : curr->prev;
        }
        // Удаляем curr
        cl_item *to_del = curr;
        curr = direction ? curr->next : curr->prev;

        to_del->prev->next = to_del->next;
        to_del->next->prev = to_del->prev;

        if (to_del == work->head) work->head = curr;
        if (to_del == work->tail) work->tail = curr->prev;

        free(to_del);
    }

    // Остался один элемент
    tvalue survivor = work->head->value;

    // Вычисляем индекс в оригинальном списке
    int idx = 0;
    cl_item *p = from->head;
    do {
        if (p->value == survivor) break;
        idx++;
        p = p->next;
    } while (p != from->head);

    *result_id = idx;
    *result_value = survivor;

    free(work->head);
    free(work);

    return 0;
}