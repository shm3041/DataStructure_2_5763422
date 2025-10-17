/*
자료구조에서 탐색횟수를 비교하기 위해 아래의 조건에 맞는 프로그램을 작성해야 한다.
언어는 C언어를 사용해야 한다. 출력은 한국어로 한다.
배열1은 중복되지 않은 0~10000사이의 난수 1000개를 생성하여 정렬하지 않고 난수를 삽입,
배열2는 0부터 999까지 정수를 삽입,
배열3은 999부터 0까지 정수를 삽입,
배열4는 for (int i = 0; i < 1000; i++) value[i] = i * (i % 2 + 2)로 정수를 삽입한다.
자료구조는 배열, 이진탐색트리, AVL로 정의한다.
배열에 저장된 난수를 이진탐색트리, AVL로 받은 후 저장된 데이터에 대한 탐색을 자료구조별로 1회씩 반복한다.
0~10000 사이의 난수 1000개의 난수를 발생시켜 배열, 이진탐색트리, AVL로 탐색한다. 탐색 시 탐색 횟수를 자료구조별로 기록한다.
탐색 횟수란, 노드가 찾으려는 숫자와 일치하는지 비교한 횟수이다.
이 때 데이터의 존재 여부에 관계없이 자료구조별로 탐색 횟수를 기록한다.
마지막으로 각 자료구조 별로 탐색한 횟수의 평균을 출력한다.
*/


// filename: search_compare.c
// Compile with: Visual Studio 2022 (C11)
// Purpose: Compare search cost (comparison counts) for Array (linear), BST, and AVL

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int key;
    struct Node* left;
    struct Node* right;
    int height; // used for AVL (height of subtree)
} Node;

// --------- Utility: create node ----------
Node* new_node(int key) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (!n) { fprintf(stderr, "malloc failed\n"); exit(1); }
    n->key = key;
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

// --------- BST insert (no balancing) ----------
Node* bst_insert(Node* root, int key) {
    if (!root) return new_node(key);
    if (key < root->key)
        root->left = bst_insert(root->left, key);
    else if (key > root->key)
        root->right = bst_insert(root->right, key);
    // duplicates ignored
    return root;
}

// --------- AVL helpers ----------
int height_node(Node* n) { return n ? n->height : 0; }
int max_int(int a, int b) { return a > b ? a : b; }
void update_height(Node* n) { if (n) n->height = 1 + max_int(height_node(n->left), height_node(n->right)); }
int balance_factor(Node* n) { return n ? (height_node(n->left) - height_node(n->right)) : 0; }

Node* rotate_right(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    update_height(y);
    update_height(x);
    return x;
}

Node* rotate_left(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    update_height(x);
    update_height(y);
    return y;
}

Node* avl_insert(Node* node, int key) {
    if (!node) return new_node(key);
    if (key < node->key)
        node->left = avl_insert(node->left, key);
    else if (key > node->key)
        node->right = avl_insert(node->right, key);
    else
        return node; // duplicates ignored

    update_height(node);
    int bf = balance_factor(node);

    // Left Left
    if (bf > 1 && key < node->left->key) return rotate_right(node);
    // Right Right
    if (bf < -1 && key > node->right->key) return rotate_left(node);
    // Left Right
    if (bf > 1 && key > node->left->key) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    // Right Left
    if (bf < -1 && key < node->right->key) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;
}

// --------- Search functions with comparison counting ----------
// Each search returns 1 if found, 0 if not, and increments *cmp by number of key comparisons made.
int array_search_count(const int* a, int n, int key, long long* cmp) {
    for (int i = 0; i < n; ++i) {
        (*cmp)++;
        if (a[i] == key) return 1;
    }
    return 0;
}

int bst_search_count(Node* root, int key, long long* cmp) {
    Node* cur = root;
    while (cur) {
        (*cmp)++;
        if (key == cur->key) return 1;
        else if (key < cur->key) cur = cur->left;
        else cur = cur->right;
    }
    return 0;
}

int avl_search_count(Node* root, int key, long long* cmp) {
    // same logic as BST search; AVL is BST with balancing
    return bst_search_count(root, key, cmp);
}

// --------- Free tree ----------
void free_tree(Node* root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

// --------- Make arrays as specified ----------
void make_array1_unique_random(int* out, int n, int minv, int maxv) {
    // generate unique random numbers in [minv, maxv], n <= maxv-minv+1
    int range = maxv - minv + 1;
    if (n > range) { fprintf(stderr, "range too small\n"); exit(1); }
    // Simple reservoir: create an array of all candidates, shuffle, pick first n
    int* pool = (int*)malloc(range * sizeof(int));
    if (!pool) { fprintf(stderr, "malloc fail\n"); exit(1); }
    for (int i = 0; i < range; ++i) pool[i] = minv + i;
    // Fisher-Yates shuffle partially
    for (int i = range - 1; i > 0 && range - i <= n * 3; --i) { // we can shuffle fully; range <= 10001 so ok
        int j = rand() % (i + 1);
        int tmp = pool[i]; pool[i] = pool[j]; pool[j] = tmp;
    }
    // take first n
    for (int i = 0; i < n; ++i) out[i] = pool[i];
    free(pool);
}

void make_array2_seq(int* out, int n) {
    for (int i = 0; i < n; ++i) out[i] = i;
}
void make_array3_revseq(int* out, int n) {
    for (int i = 0; i < n; ++i) out[i] = n - 1 - i;
}
void make_array4_formula(int* out, int n) {
    for (int i = 0; i < n; ++i) out[i] = i * (i % 2 + 2);
}

// --------- Run one experiment for a given array ----------
typedef struct Result {
    long long array_total_cmp; // total comparisons across 1000 searches
    long long bst_total_cmp;
    long long avl_total_cmp;
} Result;

Result run_one_array_experiment(const int* arr, int n, int queries, int minq, int maxq) {
    Result r = { 0,0,0 };
    // Build BST and AVL from arr (in insertion order)
    Node* bst = NULL;
    Node* avl = NULL;
    for (int i = 0; i < n; ++i) {
        bst = bst_insert(bst, arr[i]);
        avl = avl_insert(avl, arr[i]);
    }
    // perform queries
    for (int q = 0; q < queries; ++q) {
        int key = rand() % (maxq - minq + 1) + minq;
        array_search_count(arr, n, key, &r.array_total_cmp);
        bst_search_count(bst, key, &r.bst_total_cmp);
        avl_search_count(avl, key, &r.avl_total_cmp);
    }
    free_tree(bst);
    free_tree(avl);
    return r;
}

int main(void) {
    srand((unsigned)time(NULL));

    const int N = 1000;           // array size
    const int QUERIES = 1000;     // searches per experiment
    const int MINV = 0, MAXV = 10000;

    int* arr = (int*)malloc(sizeof(int) * N);
    if (!arr) { fprintf(stderr, "malloc fail\n"); return 1; }

    // We'll store per-array results and then compute averages
    Result results[4];

    // Array 1: unique random 0..10000 (unsorted)
    make_array1_unique_random(arr, N, MINV, MAXV);
    results[0] = run_one_array_experiment(arr, N, QUERIES, MINV, MAXV);
    printf("Array1 (unique random 0..10000) -> total cmp (array/BST/AVL): %lld / %lld / %lld\n",
        results[0].array_total_cmp, results[0].bst_total_cmp, results[0].avl_total_cmp);
    printf("  avg cmp per search: array=%.4f, bst=%.4f, avl=%.4f\n\n",
        (double)results[0].array_total_cmp / QUERIES,
        (double)results[0].bst_total_cmp / QUERIES,
        (double)results[0].avl_total_cmp / QUERIES);

    // Array 2: 0..999
    make_array2_seq(arr, N);
    results[1] = run_one_array_experiment(arr, N, QUERIES, MINV, MAXV);
    printf("Array2 (0..999 increasing) -> total cmp (array/BST/AVL): %lld / %lld / %lld\n",
        results[1].array_total_cmp, results[1].bst_total_cmp, results[1].avl_total_cmp);
    printf("  avg cmp per search: array=%.4f, bst=%.4f, avl=%.4f\n\n",
        (double)results[1].array_total_cmp / QUERIES,
        (double)results[1].bst_total_cmp / QUERIES,
        (double)results[1].avl_total_cmp / QUERIES);

    // Array 3: 999..0
    make_array3_revseq(arr, N);
    results[2] = run_one_array_experiment(arr, N, QUERIES, MINV, MAXV);
    printf("Array3 (999..0 decreasing) -> total cmp (array/BST/AVL): %lld / %lld / %lld\n",
        results[2].array_total_cmp, results[2].bst_total_cmp, results[2].avl_total_cmp);
    printf("  avg cmp per search: array=%.4f, bst=%.4f, avl=%.4f\n\n",
        (double)results[2].array_total_cmp / QUERIES,
        (double)results[2].bst_total_cmp / QUERIES,
        (double)results[2].avl_total_cmp / QUERIES);

    // Array 4: formula
    make_array4_formula(arr, N);
    results[3] = run_one_array_experiment(arr, N, QUERIES, MINV, MAXV);
    printf("Array4 (i*(i%%2+2)) -> total cmp (array/BST/AVL): %lld / %lld / %lld\n",
        results[3].array_total_cmp, results[3].bst_total_cmp, results[3].avl_total_cmp);
    printf("  avg cmp per search: array=%.4f, bst=%.4f, avl=%.4f\n\n",
        (double)results[3].array_total_cmp / QUERIES,
        (double)results[3].bst_total_cmp / QUERIES,
        (double)results[3].avl_total_cmp / QUERIES);

    // Final averages across 4 arrays (average comparisons per search)
    double array_avg = 0.0, bst_avg = 0.0, avl_avg = 0.0;
    for (int i = 0; i < 4; ++i) {
        array_avg += (double)results[i].array_total_cmp / QUERIES;
        bst_avg += (double)results[i].bst_total_cmp / QUERIES;
        avl_avg += (double)results[i].avl_total_cmp / QUERIES;
    }
    array_avg /= 4.0;
    bst_avg /= 4.0;
    avl_avg /= 4.0;

    printf("=== Final average comparisons per search (averaged across Array1..Array4) ===\n");
    printf("Array (linear): %.4f comparisons/search\n", array_avg);
    printf("BST:            %.4f comparisons/search\n", bst_avg);
    printf("AVL:            %.4f comparisons/search\n", avl_avg);

    free(arr);
    return 0;
}
