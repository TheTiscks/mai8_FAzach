/*
Функция meld_bh сливает две двоичные кучи (binary heap) с разрушением исходных (first и second становятся пустыми).
Результат — новая куча в result (динамически выделенная).
Алгоритм:

Общая ёмкость новой кучи = first->size + second->size.
Выделяем новый массив под элементы.
Копируем элементы из first и second.
Применяем bottom-up heapify ко всем элементам (O(n)).
Обнуляем first и second (освобождаем их массивы).
result получает новый массив, размер и компаратор (берём из first, предполагаем одинаковый).
*/


#include <stdlib.h>
#include <string.h>

typedef int tkey;
typedef int tvalue;

typedef struct bh_item {
    tkey key;
    tvalue value;
} bh_item;

typedef struct bh {
    bh_item *array;
    size_t size;
    size_t capacity;
    int (*comparer)(tvalue left, tvalue right);  // <0 если left < right (min-heap)
} bh;

/* Вспомогательная: bottom-up heapify */
static void heapify_up(bh *h, size_t idx) {
    while (idx > 0) {
        size_t p = (idx - 1) / 2;
        if (h->comparer(h->array[idx].value, h->array[p].value) >= 0) break;
        bh_item tmp = h->array[idx];
        h->array[idx] = h->array[p];
        h->array[p] = tmp;
        idx = p;
    }
}

int meld_bh(bh *first, bh *second, bh **result) {
    if (!first || !second || !result) return -1;
    if (first->comparer != second->comparer) return -2;  // разные правила порядка

    size_t new_size = first->size + second->size;
    if (new_size == 0) {
        *result = malloc(sizeof(bh));
        if (!*result) return -3;
        (*result)->array = NULL;
        (*result)->size = 0;
        (*result)->capacity = 0;
        (*result)->comparer = first->comparer;
        return 0;
    }

    bh_item *new_array = malloc(new_size * sizeof(bh_item));
    if (!new_array) return -3;

    // Копируем из first
    if (first->size > 0) {
        memcpy(new_array, first->array, first->size * sizeof(bh_item));
    }
    // Копируем из second
    if (second->size > 0) {
        memcpy(new_array + first->size, second->array, second->size * sizeof(bh_item));
    }

    // Создаём результат
    *result = malloc(sizeof(bh));
    if (!*result) {
        free(new_array);
        return -3;
    }
    (*result)->array = new_array;
    (*result)->size = new_size;
    (*result)->capacity = new_size;
    (*result)->comparer = first->comparer;

    // Bottom-up heapify для всех элементов
    for (size_t i = 0; i < new_size; i++) {
        heapify_up(*result, i);
    }

    // Разрушаем исходные кучи
    free(first->array);
    first->array = NULL;
    first->size = first->capacity = 0;

    free(second->array);
    second->array = NULL;
    second->size = second->capacity = 0;

    return 0;
}

/* Пример использования */
#include <stdio.h>

int min_cmp(tvalue a, tvalue b) { return a - b; }

void print_bh(bh *h) {
    for (size_t i = 0; i < h->size; i++) {
        printf("%d ", h->array[i].value);
    }
    printf("\n");
}

int main() {
    bh h1 = { NULL, 0, 0, min_cmp };
    bh h2 = { NULL, 0, 0, min_cmp };

    // Заполняем h1
    h1.size = 3;
    h1.array = malloc(3 * sizeof(bh_item));
    h1.array[0] = (bh_item){0, 5};
    h1.array[1] = (bh_item){0, 10};
    h1.array[2] = (bh_item){0, 15};

    // Заполняем h2
    h2.size = 3;
    h2.array = malloc(3 * sizeof(bh_item));
    h2.array[0] = (bh_item){0, 3};
    h2.array[1] = (bh_item){0, 8};
    h2.array[2] = (bh_item){0, 12};

    bh *res = NULL;
    int code = meld_bh(&h1, &h2, &res);

    if (code == 0 && res) {
        printf("Результат слияния: ");
        print_bh(res);
        free(res->array);
        free(res);
    }

    return 0;
}