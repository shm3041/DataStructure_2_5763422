#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000
#define MAX_VALUE 10000

// ===== 노드 구조체 정의 =====
typedef struct Node {
    int key;
    struct Node* left, * right;
    int height;
} Node;

// ===== 전역 탐색 횟수 카운터 =====
int bst_count = 0;
int avl_count = 0;

// ===== 유틸리티 함수 =====
int getMax(int a, int b) { return a > b ? a : b; }
int height(Node* n) { return n ? n->height : 0; }

Node* newNode(int key) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// ===== BST 삽입 =====
Node* bst_insert(Node* root, int key) {
    if (!root) return newNode(key);
    if (key < root->key) root->left = bst_insert(root->left, key);
    else if (key > root->key) root->right = bst_insert(root->right, key);
    return root;
}

// ===== AVL 회전 =====
Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = getMax(height(y->left), height(y->right)) + 1;
    x->height = getMax(height(x->left), height(x->right)) + 1;
    return x;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = getMax(height(x->left), height(x->right)) + 1;
    y->height = getMax(height(y->left), height(y->right)) + 1;
    return y;
}

int getBalance(Node* n) {
    return n ? height(n->left) - height(n->right) : 0;
}

// ===== AVL 삽입 =====
Node* avl_insert(Node* node, int key) {
    if (!node) return newNode(key);

    if (key < node->key)
        node->left = avl_insert(node->left, key);
    else if (key > node->key)
        node->right = avl_insert(node->right, key);
    else
        return node;

    node->height = 1 + getMax(height(node->left), height(node->right));
    int balance = getBalance(node);

    // LL
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);
    // RR
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);
    // LR
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // RL
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

// ===== 탐색 함수 =====
int search_array(int arr[], int size, int key) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        count++;
        if (arr[i] == key) break;
    }
    return count;
}

void search_bst(Node* root, int key) {
    if (!root) { bst_count++; return; }
    bst_count++;
    if (root->key == key) return;
    if (key < root->key) search_bst(root->left, key);
    else search_bst(root->right, key);
}

void search_avl(Node* root, int key) {
    if (!root) { avl_count++; return; }
    avl_count++;
    if (root->key == key) return;
    if (key < root->key) search_avl(root->left, key);
    else search_avl(root->right, key);
}

// ===== 난수 유틸 =====
int getRandomUnique(int used[], int range) {
    int r;
    do { r = rand() % range; } while (used[r]);
    used[r] = 1;
    return r;
}

// ===== 평균 탐색 테스트 =====
void test_dataset(int arr[], int size, const char* name) {
    Node* bst = NULL, * avl = NULL;

    // 트리 구성
    for (int i = 0; i < size; i++) {
        bst = bst_insert(bst, arr[i]);
        avl = avl_insert(avl, arr[i]);
    }

    double arr_avg = 0, bst_avg = 0, avl_avg = 0;

    for (int t = 0; t < 1000; t++) {
        int key = rand() % (MAX_VALUE + 1);

        arr_avg += search_array(arr, size, key);

        bst_count = 0;
        search_bst(bst, key);
        bst_avg += bst_count;

        avl_count = 0;
        search_avl(avl, key);
        avl_avg += avl_count;
    }

    printf("\n===== [%s] 데이터 =====\n", name);
    printf("배열 탐색 평균 횟수: %.2f\n", arr_avg / 1000);
    printf("이진탐색트리 평균 횟수: %.2f\n", bst_avg / 1000);
    printf("AVL 트리 평균 횟수: %.2f\n", avl_avg / 1000);
}

// ===== 메인 =====
int main() {
    srand((unsigned)time(NULL));

    int arr1[N], arr2[N], arr3[N], arr4[N];
    int used[MAX_VALUE + 1] = { 0 };

    // 배열1: 중복 없는 0~10000 난수
    for (int i = 0; i < N; i++)
        arr1[i] = getRandomUnique(used, MAX_VALUE + 1);

    // 배열2: 0 ~ 999
    for (int i = 0; i < N; i++)
        arr2[i] = i;

    // 배열3: 999 ~ 0
    for (int i = 0; i < N; i++)
        arr3[i] = N - 1 - i;

    // 배열4: i * (i % 2 + 2)
    for (int i = 0; i < N; i++)
        arr4[i] = i * (i % 2 + 2);

    printf("==== 자료구조별 탐색 횟수 비교 프로그램 ====\n");

    test_dataset(arr1, N, "배열1 (무작위)");
    test_dataset(arr2, N, "배열2 (오름차순)");
    test_dataset(arr3, N, "배열3 (내림차순)");
    test_dataset(arr4, N, "배열4 (규칙적 패턴)");

    return 0;
}
