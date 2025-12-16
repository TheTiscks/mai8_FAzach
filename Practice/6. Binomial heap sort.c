/*
Биномиальная куча (binomial heap) не поддерживает классическую операцию «отсортировать кучу за O(n log n)» как в обычной двоичной куче, потому что её основное преимущество — эффективное слияние (merge) за O(log n).
Однако единственный эффективный способ получить отсортированный вывод из биномиальной кучи — это многократно извлекать минимум (extract-min), пока куча не опустеет.
Каждая операция extract-min в биномиальной куче стоит O(log n), а всего их n штук → общая сложность O(n log n), что оптимально для сортировки на основе сравнений.
Это и есть стандартная «сортировка биномиальной кучей» (binomial heap sort), аналогичная heap sort для двоичных куч.
Алгоритм:

Построить биномиальную кучу из n элементов (можно за O(n), если использовать bottom-up слияние).
n раз вызвать extract-min и записать результат в массив.
Массив будет отсортирован по возрастанию.


Сложность: O(n log n) в худшем и среднем случае.
Построение кучи через n вставок — O(n log n), но существует O(n)-вариант (массивом деревьев).
Основная идея сортировки — n раз extract-min.
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    int degree;
    struct Node *child, *sibling, *parent;
} Node;

typedef struct {
    Node *head;  // список корней деревьев
} BinHeap;

// Вспомогательные функции
Node* create_node(int key) {
    Node *n = malloc(sizeof(Node));
    n->key = key;
    n->degree = 0;
    n->child = n->sibling = n->parent = NULL;
    return n;
}

BinHeap* create_heap() {
    BinHeap *h = malloc(sizeof(BinHeap));
    h->head = NULL;
    return h;
}

// Слияние двух биномиальных деревьев одинаковой степени
Node* merge_trees(Node *a, Node *b) {
    if (a->key > b->key) { Node *t = a; a = b; b = t; }
    b->parent = a;
    b->sibling = a->child;
    a->child = b;
    a->degree++;
    return a;
}

// Объединение двух куч (O(log n))
BinHeap* union_heaps(BinHeap *h1, BinHeap *h2) {
    if (!h1->head) return h2;
    if (!h2->head) return h1;

    BinHeap *res = create_heap();
    Node *p1 = h1->head, *p2 = h2->head;
    Node *prev = NULL, *curr;

    while (p1 && p2) {
        if (p1->degree <= p2->degree) {
            curr = p1; p1 = p1->sibling;
        } else {
            curr = p2; p2 = p2->sibling;
        }
        if (!prev) res->head = curr;
        else prev->sibling = curr;
        prev = curr;
    }
    prev->sibling = p1 ? p1 : p2;

    // Слияние деревьев одинаковой степени
    Node *pp = NULL;
    curr = res->head;
    Node *next = curr ? curr->sibling : NULL;

    while (next) {
        if (curr->degree != next->degree || (next->sibling && next->sibling->degree == curr->degree)) {
            pp = curr;
            curr = next;
        } else {
            if (curr->key <= next->key) {
                curr->sibling = next->sibling;
                merge_trees(curr, next);
            } else {
                if (pp) pp->sibling = next;
                else res->head = next;
                merge_trees(next, curr);
                curr = next;
            }
        }
        next = curr->sibling;
    }
    return res;
}

// Вставка элемента
void insert(BinHeap *h, int key) {
    BinHeap *temp = create_heap();
    temp->head = create_node(key);
    BinHeap *merged = union_heaps(h, temp);
    h->head = merged->head;
    free(temp);
    free(merged);
}

// Извлечение минимума (O(log n))
int extract_min(BinHeap *h) {
    if (!h->head) return -1; // пусто

    // Находим дерево с минимальным корнем
    Node *min_node = h->head, *prev_min = NULL;
    Node *p = h->head->sibling, *prev = h->head;
    while (p) {
        if (p->key < min_node->key) {
            min_node = p;
            prev_min = prev;
        }
        prev = p;
        p = p->sibling;
    }

    // Удаляем min_node из списка корней
    if (prev_min) prev_min->sibling = min_node->sibling;
    else h->head = min_node->sibling;

    // Разворачиваем детей min_node в новую кучу
    BinHeap *child_heap = create_heap();
    Node *child = min_node->child;
    Node *rev = NULL;
    while (child) {
        Node *next = child->sibling;
        child->sibling = rev;
        child->parent = NULL;
        rev = child;
        child = next;
    }
    child_heap->head = rev;

    // Сливаем с основной кучей
    BinHeap *merged = union_heaps(h, child_heap);
    h->head = merged->head;
    free(child_heap);
    free(merged);

    int min_key = min_node->key;
    free(min_node);
    return min_key;
}

// === СОРТИРОВКА БИНОМИАЛЬНОЙ КУЧЕЙ ===
void binomial_heap_sort(int arr[], int n) {
    BinHeap *h = create_heap();

    // Вставка всех элементов (можно оптимизировать до O(n), но для простоты O(n log n))
    for (int i = 0; i < n; i++)
        insert(h, arr[i]);

    // Извлечение по возрастанию
    for (int i = 0; i < n; i++)
        arr[i] = extract_min(h);
}

// Тест
int main() {
    int a[] = {5, 2, 8, 1, 9, 3};
    int n = 6;

    binomial_heap_sort(a, n);

    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);  // 1 2 3 5 8 9
    printf("\n");

    return 0;
}