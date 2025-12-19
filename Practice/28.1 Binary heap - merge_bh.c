/*
Двоичная куча (binary heap) — массив, представленный как полное бинарное дерево.
Слияние двух куч без разрушения исходных:

Копируем обе кучи в новые массивы.
Объединяем массивы в один (размер = sum размеров).
Применяем heapify снизу вверх (bottom-up) для восстановления свойства кучи за O(n).

Это оптимально (O(n)), в отличие от n вставок O(n log n).
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
    int (*comparer)(tvalue left, tvalue right);  // <0 если left < right
} bh;

/* Вспомогательная: heapify снизу вверх */
static void heapify_bottom_up(bh *h, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (h->comparer(h->array[idx].value, h->array[parent].value) >= 0)
            break;
        bh_item temp = h->array[idx];
        h->array[idx] = h->array[parent];
        h->array[parent] = temp;
        idx = parent;
    }
}

/* Основная функция слияния */
int merge_bh(bh *target, bh *source) {
    if (!target || !source || !source->array) {
        return -1;
    }

    // Если source пуст — ничего не делаем
    if (source->size == 0) {
        return 0;
    }

    // Проверяем совместимость компаратора
    if (target->comparer != source->comparer && target->size > 0) {
        return -2;  // разные правила порядка
    }

    int (*cmp)(tvalue, tvalue) = target->comparer ? target->comparer : source->comparer;

    // Выделяем место под объединённый массив
    size_t new_size = target->size + source->size;
    bh_item *new_array = realloc(target->array, new_size * sizeof(bh_item));
    if (!new_array) {
        return -3;  // ошибка памяти
    }

    // Копируем элементы source в конец
    memcpy(new_array + target->size, source->array, source->size * sizeof(bh_item));

    target->array = new_array;
    target->size = new_size;
    target->comparer = cmp;

    // Heapify снизу вверх для всех новых элементов
    for (size_t i = target->size - source->size; i < target->size; i++) {
        heapify_bottom_up(target, i);
    }

    // source остаётся нетронутым, как требуется
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

    // Заполняем h1: 5 10 15
    h1.array = malloc(3 * sizeof(bh_item));
    h1.array[0] = (bh_item){0, 5};
    h1.array[1] = (bh_item){0, 10};
    h1.array[2] = (bh_item){0, 15};
    h1.size = 3;

    // Заполняем h2: 3 8 12
    h2.array = malloc(3 * sizeof(bh_item));
    h2.array[0] = (bh_item){0, 3};
    h2.array[1] = (bh_item){0, 8};
    h2.array[2] = (bh_item){0, 12};
    h2.size = 3;

    printf("h1: "); print_bh(&h1);
    printf("h2: "); print_bh(&h2);

    int code = merge_bh(&h1, &h2);

    if (code == 0) {
        printf("После слияния h1: "); print_bh(&h1);
    } else {
        printf("Ошибка: %d\n", code);
    }

    // Очистка
    free(h1.array);
    free(h2.array);

    return 0;
}