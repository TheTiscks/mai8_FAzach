#include <stdio.h>
#include <stdlib.h>

typedef int tvalue;

typedef struct ul_item {
    tvalue value;
    struct ul_item *next;
} ul_item;

typedef struct ul {
    ul_item *head;
} ul;

/*
 * Возвращает количество найденных элементов или код ошибки:
 * -1 — ошибка выделения памяти
 * -2 — один из параметров NULL
 */
int find_by(ul *where, int (*predicate)(tvalue value), ul *result) {
    if (!where || !predicate || !result) {
        return -2;
    }

    result->head = NULL;                // гарантируем чистый результат
    ul_item **tail = &result->head;     // указатель на поле next последнего элемента
    int count = 0;
    ul_item *curr = where->head;

    while (curr) {
        if (predicate(curr->value)) {
            ul_item *new_item = malloc(sizeof(ul_item));
            if (!new_item) {
                // При ошибке памяти — очищаем уже построенный result (по желанию преподавателя)
                // Здесь упрощённо — просто возвращаем -1
                return -1;
            }
            new_item->value = curr->value;
            new_item->next = NULL;

            *tail = new_item;           // добавляем в конец
            tail = &new_item->next;     // сдвигаем tail на следующее поле next
            count++;
        }
        curr = curr->next;
    }
    return count;
}

/* Примеры предикатов */
int is_positive(tvalue v) { return v > 0; }
int is_even(tvalue v)     { return v % 2 == 0; }

/* Вспомогательная функция для теста */
void push_front(ul *list, tvalue val) {
    ul_item *n = malloc(sizeof(ul_item));
    n->value = val;
    n->next = list->head;
    list->head = n;
}

/* Функция печати (удобно для отладки на зачёте) */
void print_list(ul *list) {
    for (ul_item *p = list->head; p; p = p->next)
        printf("%d ", p->value);
    printf("\n");
}

int main() {
    ul source = { NULL };

    push_front(&source, -3);
    push_front(&source, 4);
    push_front(&source, -1);
    push_front(&source, 8);
    push_front(&source, 0);
    // source: 0 -> 8 -> -1 -> 4 -> -3

    ul positive = { NULL };
    ul even = { NULL };

    int cnt1 = find_by(&source, is_positive, &positive);
    int cnt2 = find_by(&source, is_even, &even);

    printf("Положительных: %d\n", cnt1 >= 0 ? cnt1 : cnt1);
    if (cnt1 >= 0) {
        printf("Список: ");
        print_list(&positive);  // 8 4
    }

    printf("Чётных: %d\n", cnt2 >= 0 ? cnt2 : cnt2);
    if (cnt2 >= 0) {
        printf("Список: ");
        print_list(&even);      // 0 8 4
    }

    // Освобождение памяти (обязательно показать на зачёте)
    // ... (можно написать clear_list)

    return 0;
}