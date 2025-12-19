/*
Функция reconstruct_ht пересобирает хеш-таблицу с цепочками при изменении размера или хеш-функции.

Если new_size == 0 — используем старый размер таблицы.
Если hash_function == NULL — используем старую хеш-функцию (сохраняем указатель из table).
Выделяем новый массив цепочек размером new_size (или старым).
Проходим по всем элементам старой таблицы, для каждого:
Вычисляем новый индекс с новой хеш-функцией и новым размером.
Перевешиваем элемент в новую цепочку (в начало — просто и быстро).

Освобождаем старый массив цепочек, обновляем поля таблицы.
*/

#include <stdlib.h>

typedef int tkey;
typedef int tvalue;

typedef struct ht_item {
    tkey key;
    tvalue value;
    struct ht_item *next;
} ht_item;

typedef struct ht {
    ht_item **buckets;          // массив указателей на цепочки
    size_t size;                // текущий размер таблицы
    int (*hash_function)(tkey); // текущая хеш-функция
} ht;

/**
 * Возвращает:
 *  0 — успех
 * -1 — table == NULL
 * -2 — ошибка выделения памяти
 */
int reconstruct_ht(ht *table, int (*hash_function)(tkey), size_t new_size) {
    if (!table) {
        return -1;
    }

    size_t actual_size = new_size ? new_size : table->size;
    int (*actual_hash)(tkey) = hash_function ? hash_function : table->hash_function;

    if (actual_size == 0) {
        actual_size = 1;  // минимальный размер
    }

    // Выделяем новый массив buckets
    ht_item **new_buckets = calloc(actual_size, sizeof(ht_item*));
    if (!new_buckets) {
        return -2;
    }

    // Перераспределяем все элементы
    for (size_t i = 0; i < table->size; i++) {
        ht_item *curr = table->buckets[i];
        while (curr) {
            ht_item *next = curr->next;

            // Новый индекс
            size_t index = (size_t)actual_hash(curr->key) % actual_size;

            // Вставляем в начало новой цепочки
            curr->next = new_buckets[index];
            new_buckets[index] = curr;

            curr = next;
        }
    }

    // Освобождаем старый массив buckets
    free(table->buckets);

    // Обновляем таблицу
    table->buckets = new_buckets;
    table->size = actual_size;
    table->hash_function = actual_hash;

    return 0;
}

/* Пример использования */
#include <stdio.h>

int simple_hash(tkey k) {
    return k >= 0 ? k : -k;
}

void insert(ht *t, tkey k, tvalue v) {
    size_t index = (size_t)simple_hash(k) % t->size;
    ht_item *n = malloc(sizeof(ht_item));
    n->key = k; n->value = v; n->next = t->buckets[index];
    t->buckets[index] = n;
}

void print_ht(ht *t) {
    for (size_t i = 0; i < t->size; i++) {
        printf("%zu: ", i);
        ht_item *p = t->buckets[i];
        while (p) {
            printf("(%d,%d) ", p->key, p->value);
            p = p->next;
        }
        printf("\n");
    }
}

int main() {
    ht table = { NULL, 0, NULL };

    // Инициализация начальной таблицы
    table.size = 5;
    table.hash_function = simple_hash;
    table.buckets = calloc(table.size, sizeof(ht_item*));

    insert(&table, 1, 100);
    insert(&table, 6, 600);  // коллизия с 1
    insert(&table, 11, 1100); // коллизия
    insert(&table, 2, 200);

    printf("До реконструкции:\n");
    print_ht(&table);

    // Реконструкция с новым размером
    int code = reconstruct_ht(&table, NULL, 10);  // без смены хеш-функции

    if (code == 0) {
        printf("\nПосле реконструкции (size=10):\n");
        print_ht(&table);
    }

    // Освобождение (упрощённо)
    for (size_t i = 0; i < table.size; i++) {
        ht_item *p = table.buckets[i];
        while (p) {
            ht_item *next = p->next;
            free(p);
            p = next;
        }
    }
    free(table.buckets);

    return 0;
}