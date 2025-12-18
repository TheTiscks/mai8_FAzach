/*
Функция реализует многофакторную сортировку массива целых чисел по нескольким предикатам с весами.
Для каждого элемента item вычисляется «рейтинг»:

ording = Σ (fix(pred_i(item)) * coeff_i)

где

pred_i(item) — предикат, возвращающий ненулевое (true) или 0 (false)
fix(x) = 0, если x == 0, иначе 1
coeff_i — вес предиката (вещественное в [0.01 .. 5.00])
Суммирование по всем переданным предикатам

Параметр ascending:

если >0 — сортировка по возрастанию рейтинга
иначе — по убыванию

Список предикатов заканчивается NULL.
Мы не модифицируем исходный массив values, а заполняем массив result отсортированными значениями (копиями элементов).
Реализация:

Создаём массив структур { value, ording } размером count.
Для каждого элемента вычисляем ording по всем предикатам (va_list).
Сортируем этот массив с помощью qsort и кастомного компаратора (учитывающего ascending).
Копируем отсортированные значения в result.
*/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

// Структура для предиката с весом
typedef struct {
    int (*func)(int);   // указатель на предикат
    double coeff;       // вес [0.01 .. 5.00]
} Predicate;

typedef struct {
    int value;
    double ording;
} Item;

static int fix(int x) {
    return x != 0;
}

static int compare(const void *a, const void *b, int ascending) {
    const Item *ia = a;
    const Item *ib = b;
    if (ia->ording < ib->ording) return ascending > 0 ? -1 : 1;
    if (ia->ording > ib->ording) return ascending > 0 ? 1 : -1;
    return 0;
}

static int cmp_asc(const void *a, const void *b) {
    return compare(a, b, 1);
}

static int cmp_desc(const void *a, const void *b) {
    return compare(a, b, -1);
}

int sort_by_predicates(int *values, size_t values_count,
                       int *result, int ascending, ...) {
    if (!values || !result || values_count == 0) return -1;

    // Создаём вспомогательный массив
    Item *items = malloc(values_count * sizeof(Item));
    if (!items) return -1;

    // Копируем значения и вычисляем ording
    va_list ap;
    va_start(ap, ascending);

    for (size_t i = 0; i < values_count; i++) {
        items[i].value = values[i];
        items[i].ording = 0.0;

        const Predicate *pred;
        while ((pred = va_arg(ap, const Predicate*)) != NULL) {
            if (pred->func && pred->coeff >= 0.01 && pred->coeff <= 5.00) {
                items[i].ording += fix(pred->func(values[i])) * pred->coeff;
            }
        }
    }
    va_end(ap);

    // Сортируем
    qsort(items, values_count, sizeof(Item),
          ascending > 0 ? cmp_asc : cmp_desc);

    // Заполняем результат
    for (size_t i = 0; i < values_count; i++) {
        result[i] = items[i].value;
    }

    free(items);
    return 0;
}

/* Пример использования */
int is_even(int x)      { return x % 2 == 0; }
int is_positive(int x)  { return x > 0; }
int is_multiple_of_5(int x) { return x % 5 == 0; }

int main() {
    int values[] = {10, -3, 15, 7, -20, 0, 25, 8};
    size_t n = sizeof(values)/sizeof(values[0]);
    int result[8];

    Predicate p1 = {is_even, 1.0};
    Predicate p2 = {is_positive, 2.5};
    Predicate p3 = {is_multiple_of_5, 3.0};

    sort_by_predicates(values, n, result, 1,   // по возрастанию
                       &p1, &p2, &p3, NULL);

    for (size_t i = 0; i < n; i++)
        printf("%d ", result[i]);
    printf("\n");

    return 0;
}