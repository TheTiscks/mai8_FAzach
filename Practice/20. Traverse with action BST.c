/*
Функция выполняет обход BST в заданном порядке (prefix, infix, postfix) и для каждого узла вызывает пользовательскую функцию action(key, value, depth).
Перечисление traverse_rule определяет тип обхода:

prefix — корень → левое → правое
infix — левое → корень → правое
postfix — левое → правое → корень

Глубина корня = 0.
Реализация — рекурсивная, с передачей текущей глубины.
*/


#include <stdlib.h>

typedef int tkey;
typedef int tvalue;

// Перечисление видов обхода
typedef enum {
    prefix,
    infix,
    postfix
} traverse_rule;

// Элемент BST (ключ и значение разделены)
typedef struct bst_item {
    tkey key;
    tvalue value;
    struct bst_item *left;
    struct bst_item *right;
} bst_item;

// Структура дерева
typedef struct bst {
    bst_item *root;
} bst;

/**
 * action — обратный вызов: принимает ключ, значение и глубину относительно корня (корень = 0)
 * Возвращает:
 *  0 — успех
 * -1 — where == NULL или action == NULL
 * -2 — дерево пустое (но это не ошибка, просто ничего не делаем)
 */
int traverse_with_action(bst *tree, traverse_rule rule,
                         void (*action)(tkey, tvalue, size_t)) {
    if (!tree || !action) {
        return -1;
    }

    if (!tree->root) {
        return -2;  // пустое дерево
    }

    // Вспомогательная рекурсивная функция
    void traverse(bst_item *node, traverse_rule r, size_t depth) {
        if (!node) return;

        if (r == prefix) {
            action(node->key, node->value, depth);
        }

        traverse(node->left, r, depth + 1);

        if (r == infix) {
            action(node->key, node->value, depth);
        }

        traverse(node->right, r, depth + 1);

        if (r == postfix) {
            action(node->key, node->value, depth);
        }
    }

    traverse(tree->root, rule, 0);
    return 0;
}

/* Пример использования */
#include <stdio.h>

void print_node(tkey k, tvalue v, size_t d) {
    printf("depth=%zu key=%d value=%d\n", d, k, v);
}

void insert(bst *t, tkey k, tvalue v) {  // простая вставка для теста
    bst_item **p = &t->root;
    while (*p) {
        if (k < (*p)->key) p = &(*p)->left;
        else p = &(*p)->right;
    }
    *p = malloc(sizeof(bst_item));
    (*p)->key = k;
    (*p)->value = v;
    (*p)->left = (*p)->right = NULL;
}

int main() {
    bst tree = { NULL };

    insert(&tree, 50, 500);
    insert(&tree, 30, 300);
    insert(&tree, 70, 700);
    insert(&tree, 20, 200);
    insert(&tree, 40, 400);

    printf("Prefix:\n");
    traverse_with_action(&tree, prefix, print_node);

    printf("\nInfix:\n");
    traverse_with_action(&tree, infix, print_node);

    printf("\nPostfix:\n");
    traverse_with_action(&tree, postfix, print_node);

    return 0;
}