/*
Фибоначчиевая куча (Fibonacci heap) — одна из самых эффективных сливных куч (амортизировано O(1) для insert, decrease-key, merge и O(log n) для delete-min/extract-min).
Удаление произвольного элемента реализуется через стандартный подход:

Уменьшаем ключ элемента до -∞ (минимально возможного значения).
Вызываем extract-min — это удалит именно этот элемент как новый минимум.
Возвращаем его значение через *value.

Для простоты реализации используем минимальное значение INT_MIN как -∞.
*/


#include <stdlib.h>
#include <limits.h>

typedef int tkey;
typedef int tvalue;

typedef struct fh_item {
    tkey key;
    tvalue value;
    int degree;
    struct fh_item *parent;
    struct fh_item *child;
    struct fh_item *left;
    struct fh_item *right;
    int mark;
} fh_item;

typedef struct fh {
    fh_item *min;
    int n;
} fh;

/* Вспомогательные функции (упрощённая реализация) */
static void fh_link(fh *h, fh_item *y, fh_item *x) {
    // Удаляем y из корневого списка
    y->left->right = y->right;
    y->right->left = y->left;

    // Делаем y ребёнком x
    y->parent = x;
    if (!x->child) {
        x->child = y->left = y->right = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
    y->mark = 0;
}

static void fh_consolidate(fh *h) {
    int max_deg = 32; // достаточно для int
    fh_item **A = calloc(max_deg, sizeof(fh_item*));

    fh_item *w = h->min;
    fh_item *start = w;
    do {
        fh_item *x = w;
        w = w->right;
        int d = x->degree;
        while (A[d]) {
            fh_item *y = A[d];
            if (x->key > y->key) {
                fh_item *temp = x; x = y; y = temp;
            }
            fh_link(h, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
    } while (w != start);

    h->min = NULL;
    for (int i = 0; i < max_deg; i++) {
        if (A[i]) {
            if (!h->min || A[i]->key < h->min->key) h->min = A[i];
        }
    }
    free(A);
}

static fh_item* fh_extract_min(fh *h) {
    fh_item *z = h->min;
    if (z) {
        // Добавляем детей в корневой список
        if (z->child) {
            fh_item *x = z->child;
            do {
                fh_item *next = x->right;
                x->left = h->min;
                x->right = h->min->right;
                h->min->right->left = x;
                h->min->right = x;
                x->parent = NULL;
                x = next;
            } while (x != z->child);
        }
        // Удаляем z из корневого списка
        z->left->right = z->right;
        z->right->left = z->left;
        if (z == z->right) h->min = NULL;
        else {
            h->min = z->right;
            fh_consolidate(h);
        }
        h->n--;
    }
    return z;
}

/* Основная функция: удаление произвольного элемента */
int delete_from_bmh(fh *from, tvalue *value) {
    if (!from || !value || !from->min) return -1;

    // Уменьшаем ключ до -∞
    from->min->key = INT_MIN;

    // Извлекаем минимум — это будет наш элемент
    fh_item *removed = fh_extract_min(from);
    if (!removed) return -2;

    *value = removed->value;
    free(removed);

    return 0;
}

/* Пример использования */
#include <stdio.h>

fh* fh_make_heap() {
    fh *h = malloc(sizeof(fh));
    h->min = NULL;
    h->n = 0;
    return h;
}

void fh_insert(fh *h, tkey k, tvalue v) {
    fh_item *n = malloc(sizeof(fh_item));
    n->key = k; n->value = v;
    n->degree = 0; n->mark = 0;
    n->parent = n->child = NULL;
    n->left = n->right = n;

    if (!h->min) {
        h->min = n;
    } else {
        n->left = h->min;
        n->right = h->min->right;
        h->min->right->left = n;
        h->min->right = n;
        if (k < h->min->key) h->min = n;
    }
    h->n++;
}

int main() {
    fh *heap = fh_make_heap();

    fh_insert(heap, 30, 300);
    fh_insert(heap, 10, 100);
    fh_insert(heap, 50, 500);
    fh_insert(heap, 20, 200);

    tvalue val;
    int code = delete_from_bmh(heap, &val);  // удаляем минимум (10)

    if (code == 0) {
        printf("Удалено значение: %d\n", val);  // 100
    }

    // Освобождение памяти (упрощённо)
    // ...

    return 0;
}