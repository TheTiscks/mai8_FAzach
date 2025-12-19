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

typedef struct sh_item {
    tkey key;
    tvalue value;
    struct sh_item *left;
    struct sh_item *right;
} sh_item;

typedef struct sh {
    sh_item *root;
} sh;

/* Вспомогательная функция слияния двух косых куч */
static sh_item* merge(sh_item *a, sh_item *b) {
    if (!a) return b;
    if (!b) return a;

    if (a->key > b->key) {  // min-heap, меняем если нужно max-heap
        sh_item *temp = a;
        a = b;
        b = temp;
    }

    // Меняем местами левое и правое поддерево у победителя
    sh_item *temp = a->left;
    a->left = a->right;
    a->right = temp;

    // Рекурсивно сливаем правое поддерево a с b
    a->right = merge(a->right, b);

    return a;
}

int insert_into_sh(sh *into, tkey key, tvalue value) {
    if (!into) return -1;

    sh_item *new_item = malloc(sizeof(sh_item));
    if (!new_item) return -2;

    new_item->key = key;
    new_item->value = value;
    new_item->left = NULL;
    new_item->right = NULL;

    // Сливаем новую одноэлементную кучу с into
    into->root = merge(into->root, new_item);

    return 0;
}

/* Пример использования */
#include <stdio.h>

void print_inorder(sh_item *node) {
    if (!node) return;
    print_inorder(node->left);
    printf("(%d,%d) ", node->key, node->value);
    print_inorder(node->right);
}

int main() {
    sh heap = { NULL };

    insert_into_sh(&heap, 30, 300);
    insert_into_sh(&heap, 10, 100);
    insert_into_sh(&heap, 40, 400);
    insert_into_sh(&heap, 20, 200);
    insert_into_sh(&heap, 50, 500);

    printf("Inorder обход (должен быть отсортирован по ключу):\n");
    print_inorder(heap.root);
    printf("\n");

    // Освобождение памяти (упрощённо, рекурсивно)
    // ... (можно написать free_skew_heap)

    return 0;
}