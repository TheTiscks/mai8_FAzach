/*
Сортировка массива с помощью биномиальной кучи (binomial heap sort):

Создаём пустую биномиальную кучу bmh.
Поочерёдно вставляем все элементы массива в кучу (операция insert).
Последовательно извлекаем минимум (extract-min) count раз и записываем обратно в массив.
Компаратор определяет порядок: если comparer(left, right) < 0 — left меньше right (для min-heap).

Реализуем:

вставку (union с одноэлементной кучей),
слияние двух куч,
extract-min.
*/

#include <stdlib.h>

typedef int tkey;   // в условии ключ и значение совпадают
typedef int tvalue;

typedef struct bmh_item {
    tvalue value;                   // значение (оно же ключ для сравнения)
    int degree;
    struct bmh_item *parent;
    struct bmh_item *child;
    struct bmh_item *sibling;
} bmh_item;

typedef struct bmh {
    bmh_item *head;                 // список корней деревьев (степени 0..)
} bmh;

/* Вспомогательные функции */

/* Слияние двух деревьев одинаковой степени (x становится родителем y) */
static bmh_item* link(bmh_item *x, bmh_item *y, int (*comparer)(tvalue, tvalue)) {
    if (comparer(x->value, y->value) > 0) {
        bmh_item *tmp = x; x = y; y = tmp;
    }
    y->parent = x;
    y->sibling = x->child;
    x->child = y;
    x->degree++;
    return x;
}

/* Объединение двух биномиальных куч */
static bmh_item* merge(bmh *a, bmh *b, int (*comparer)(tvalue, tvalue)) {
    if (!a->head) return b->head;
    if (!b->head) return a->head;

    bmh_item *res = NULL, *tail = NULL;
    bmh_item *pa = a->head, *pb = b->head;

    while (pa && pb) {
        if (pa->degree <= pb->degree) {
            if (!res) res = tail = pa;
            else tail = tail->sibling = pa;
            pa = pa->sibling;
        } else {
            if (!res) res = tail = pb;
            else tail = tail->sibling = pb;
            pb = pb->sibling;
        }
    }
    if (pa) tail->sibling = pa;
    if (pb) tail->sibling = pb;

    // Консолидация (слияние деревьев одинаковой степени)
    bmh_item *prev = NULL, *curr = res, *next = curr ? curr->sibling : NULL;
    while (next) {
        if (curr->degree != next->degree ||
            (next->sibling && next->sibling->degree == curr->degree)) {
            prev = curr;
            curr = next;
        } else {
            if (comparer(curr->value, next->value) <= 0) {
                curr->sibling = next->sibling;
                curr = link(curr, next, comparer);
            } else {
                if (prev) prev->sibling = next;
                else res = next;
                next->sibling = curr->sibling;
                next = link(next, curr, comparer);
                curr = next;
            }
        }
        next = curr->sibling;
    }
    return res;
}

/* Вставка элемента */
static void insert(bmh *heap, tvalue val) {
    bmh_item *item = malloc(sizeof(bmh_item));
    item->value = val;
    item->degree = 0;
    item->parent = item->child = item->sibling = NULL;

    bmh single = { item };
    heap->head = merge(heap, &single, NULL);  // comparer не нужен для одноэлементной
}

/* Извлечение минимума */
static tvalue extract_min(bmh *heap, int (*comparer)(tvalue, tvalue)) {
    if (!heap->head) return 0;  // ошибка, но не происходит

    // Находим дерево с минимальным корнем
    bmh_item *min = heap->head;
    bmh_item *prev_min = NULL;
    bmh_item *p = heap->head->sibling;
    bmh_item *prev = heap->head;
    while (p) {
        if (comparer(p->value, min->value) < 0) {
            min = p;
            prev_min = prev;
        }
        prev = p;
        p = p->sibling;
    }

    // Удаляем min из списка корней
    if (prev_min) prev_min->sibling = min->sibling;
    else heap->head = min->sibling;

    // Разворачиваем детей min в отдельную кучу
    bmh children = { NULL };
    bmh_item *child = min->child;
    while (child) {
        bmh_item *next = child->sibling;
        child->sibling = children.head;
        children.head = child;
        child->parent = NULL;
        child = next;
    }

    // Сливаем детей обратно в основную кучу
    heap->head = merge(heap, &children, comparer);

    tvalue val = min->value;
    free(min);
    return val;
}

/* Основная функция сортировки */
int sort_with_bmh(tvalue *values, size_t values_count,
                  int (*comparer)(tvalue left, tvalue right)) {
    if (!values || values_count == 0 || !comparer) return -1;

    bmh heap = { NULL };

    // Вставляем все элементы
    for (size_t i = 0; i < values_count; i++) {
        insert(&heap, values[i]);
    }

    // Извлекаем по порядку
    for (size_t i = 0; i < values_count; i++) {
        values[i] = extract_min(&heap, comparer);
    }

    // Очистка оставшихся узлов (если есть ошибка)
    while (heap.head) {
        extract_min(&heap, comparer);  // просто освобождаем
    }

    return 0;
}

/* Пример использования */
#include <stdio.h>

int asc(tvalue a, tvalue b) { return a - b; }   // a < b → отрицательное
int desc(tvalue a, tvalue b) { return b - a; }

int main() {
    tvalue arr[] = { 5, 1, 8, 3, 9, 2 };
    size_t n = sizeof(arr) / sizeof(arr[0]);

    sort_with_bmh(arr, n, asc);

    printf("По возрастанию: ");
    for (size_t i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    return 0;
}