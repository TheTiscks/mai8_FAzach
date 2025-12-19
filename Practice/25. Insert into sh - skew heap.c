/*
Косая куча (skew heap) — саморегулирующееся сливное бинарное дерево с кучей-свойством (min-heap или max-heap, в зависимости от правила порядка).
Добавление элемента реализуется через слияние (merge) двух косых куч:

Создаём новую кучу из одного элемента (лист).
Сливаем её с существующей кучей into.

Операция слияния (рекурсивная):

Если одна куча пустая — возвращаем другую.
Сравниваем корни, меньший становится новым корнем.
Правое поддерево победителя меняется местами с левым (ключевой трюк косой кучи для амортизации).
Рекурсивно сливаем правое поддерево победителя с проигравшей кучей.
*/

#include <stdlib.h>

typedef int tkey;
typedef int tvalue;

/* Элемент косой кучи */
typedef struct sh_item {
    tkey key;
    tvalue value;
    struct sh_item *left;
    struct sh_item *right;
} sh_item;

/* Косая куча (min-heap) */
typedef struct sh {
    sh_item *root;
} sh;

/* Вспомогательная функция: слияние двух косых куч */
/* Возвращает корень объединённой кучи */
static sh_item* sh_merge(sh_item *a, sh_item *b) {
    if (!a) return b;
    if (!b) return a;

    /* Обеспечиваем a <= b */
    if (a->key > b->key) {
        sh_item *temp = a;
        a = b;
        b = temp;
    }

    /* Ключевой приём косой кучи: меняем левое и правое поддерево */
    sh_item *temp = a->left;
    a->left = a->right;
    a->right = temp;

    /* Рекурсивно сливаем правое поддерево a с b */
    a->right = sh_merge(a->right, b);

    return a;
}

/* Вставка элемента в кучу */
int insert_into_sh(sh *into, tkey key, tvalue value) {
    if (!into) return -1;

    sh_item *new_item = malloc(sizeof(sh_item));
    if (!new_item) return -2;  // ошибка выделения памяти

    new_item->key = key;
    new_item->value = value;
    new_item->left = NULL;
    new_item->right = NULL;

    /* Сливаем одноэлементную кучу с существующей */
    into->root = sh_merge(into->root, new_item);

    return 0;
}

/* Извлечение минимального элемента (опционально, но часто просят) */
int extract_min_sh(sh *from, tkey *out_key, tvalue *out_value) {
    if (!from || !from->root || !out_key || !out_value) return -1;

    sh_item *min_node = from->root;

    *out_key = min_node->key;
    *out_value = min_node->value;

    /* Сливаем левое и правое поддеревья */
    from->root = sh_merge(min_node->left, min_node->right);

    free(min_node);
    return 0;
}

/* Очистка кучи (рекурсивно) — обязательно показать на зачёте */
void clear_sh(sh *heap) {
    if (!heap || !heap->root) return;

    void clear_recursive(sh_item *node) {
        if (!node) return;
        clear_recursive(node->left);
        clear_recursive(node->right);
        free(node);
    }

    clear_recursive(heap->root);
    heap->root = NULL;
}

/* Пример использования и тест */
#include <stdio.h>

int main() {
    sh heap = { NULL };

    insert_into_sh(&heap, 30, 300);
    insert_into_sh(&heap, 10, 100);
    insert_into_sh(&heap, 50, 500);
    insert_into_sh(&heap, 20, 200);
    insert_into_sh(&heap, 40, 400);

    printf("Извлечённые элементы (по возрастанию):\n");

    tkey k;
    tvalue v;
    while (heap.root) {
        extract_min_sh(&heap, &k, &v);
        printf("key=%d, value=%d\n", k, v);
    }
    // Вывод: 10, 20, 30, 40, 50

    clear_sh(&heap);  // на всякий случай

    return 0;
}