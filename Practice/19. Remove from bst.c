/*
Удаление узла из бинарного дерева поиска (BST) по ключу — классическая операция.
Случаи:

Узел — лист → просто освобождаем.
Узел имеет одного ребёнка → «перевешиваем» ребёнка на место родителя.
Узел имеет двух детей → находим минимальный узел в правом поддереве (преемник), копируем его значение в удаляемый узел, затем рекурсивно удаляем преемник (у него не более одного ребёнка).

Функция возвращает 0 при успехе, копирует значение в *result и освобождает узел.
При отсутствии ключа — возвращаем ненулевой код ошибки.
*/

#include <stdlib.h>

typedef int tkey;
typedef int tvalue;

// Элемент бинарного дерева поиска
typedef struct bst_item {
    tkey key;
    tvalue value;
    struct bst_item *left;
    struct bst_item *right;
} bst_item;

// Бинарное дерево поиска
typedef struct bst {
    bst_item *root;
} bst;

// Вспомогательная функция для поиска минимального элемента
static bst_item* bst_find_min(bst_item* node) {
    while (node && node->left != NULL) {
        node = node->left;
    }
    return node;
}

// Вспомогательная рекурсивная функция удаления
static bst_item* bst_remove_recursive(bst_item* node, tkey key, int* found, tvalue** result) {
    if (node == NULL) {
        *found = 0;
        return NULL;
    }

    if (key < node->key) {
        node->left = bst_remove_recursive(node->left, key, found, result);
    }
    else if (key > node->key) {
        node->right = bst_remove_recursive(node->right, key, found, result);
    }
    else {
        // Ключ найден
        *found = 1;

        // Выделяем динамическую память для результата
        *result = (tvalue*)malloc(sizeof(tvalue));
        if (*result == NULL) {
            // Если не удалось выделить память, не удаляем узел
            *found = -1; // Специальный код ошибки
            return node;
        }

        // Копируем значение
        **result = node->value;

        // Узел с одним или нулем потомков
        if (node->left == NULL) {
            bst_item* temp = node->right;
            free(node);
            return temp;
        }
        else if (node->right == NULL) {
            bst_item* temp = node->left;
            free(node);
            return temp;
        }

        // Узел с двумя потомками
        // Находим преемника (минимальный в правом поддереве)
        bst_item* successor = bst_find_min(node->right);

        // Копируем данные преемника
        node->key = successor->key;
        node->value = successor->value;

        // Удаляем преемника рекурсивно
        node->right = bst_remove_recursive(node->right, successor->key, found, result);

        // Освобождаем память результата от рекурсивного вызова
        if (*result != NULL) {
            free(*result);
            *result = NULL;
        }

        // Снова выделяем память для правильного результата
        *result = (tvalue*)malloc(sizeof(tvalue));
        if (*result != NULL) {
            **result = node->value; // Теперь node содержит значение преемника
        }

        return node;
    }

    return node;
}

// Основная функция удаления
int remove_from_bst(bst* where, tkey key, tvalue** result) {
    // Проверка входных параметров
    if (where == NULL || result == NULL) {
        return -1; // INVALID_ARGUMENT
    }

    // Инициализируем результат
    *result = NULL;

    int found = 0;
    where->root = bst_remove_recursive(where->root, key, &found, result);

    if (found == -1) {
        return -3; // MEMORY_ALLOCATION_ERROR
    }

    if (!found) {
        return -2; // KEY_NOT_FOUND
    }

    // Проверяем, что память была выделена
    if (*result == NULL) {
        return -3; // MEMORY_ALLOCATION_ERROR
    }

    return 0; // SUCCESS
}

// Пример использования
int main() {
    // Создание дерева
    bst tree;
    tree.root = NULL;

    // Добавление элементов (реализацию добавления нужно написать отдельно)
    // ...

    // Удаление элемента
    tvalue* removed_value = NULL;
    int status = remove_from_bst(&tree, 42, &removed_value);

    if (status == 0) {
        // Использование удаленного значения
        printf("Удалено значение: %d\n", *removed_value);

        // Освобождение памяти результата
        free(removed_value);
    } else {
        printf("Ошибка: %d\n", status);
    }

    return 0;
}