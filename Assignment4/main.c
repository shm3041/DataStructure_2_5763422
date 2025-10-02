#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DATA 100

/* -------------------- BST ���� �ڷ��� ���� -------------------- */
typedef struct BSTNode {
    int key;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

BSTNode* bst_root = NULL;

BSTNode* createNode(int key) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    node->key = key;
    node->left = node->right = NULL;
    return node;
}

void bst_insert(BSTNode** root, int key) {
    if (*root == NULL) {
        *root = createNode(key);
        return;
    }
    if (key < (*root)->key)
        bst_insert(&(*root)->left, key);
    else if (key > (*root)->key)
        bst_insert(&(*root)->right, key);
    // �ߺ��� ����
}

int bst_search(BSTNode* root, int key, int* count) {
    if (root == NULL) return -1;
    (*count)++;
    if (root->key == key) return 1;
    else if (key < root->key) return bst_search(root->left, key, count);
    else return bst_search(root->right, key, count);
}

/* -------------------- �Ϲ� ���� Ʈ�� (�迭 ���, �θ�-���� idx*2, ������ idx*2+1) -------------------- */
#define MAX_TREE 2048
#define MAX_STACK 512

typedef struct {
    int data[MAX_STACK];
    int top;
} IntStack;

void initStack(IntStack* s) { s->top = -1; }
int isEmpty(IntStack* s) { return s->top == -1; }
void push(IntStack* s, int v) { s->data[++s->top] = v; }
int pop(IntStack* s) { return isEmpty(s) ? -1 : s->data[s->top--]; }
int peek(IntStack* s) { return isEmpty(s) ? -1 : s->data[s->top]; }

int parseToHeap(const char* in, char tree[], int size) {
    IntStack st; initStack(&st);
    int lastIdx = 0;
    int childFlag = 0; // 0=����, 1=������

    for (int i = 0; in[i]; ++i) {
        char c = in[i];
        if (isspace((unsigned char)c)) continue;

        if (c == '(') {
            if (lastIdx != 0) {
                push(&st, lastIdx);
                childFlag = 0;
            }
            lastIdx = 0;
        }
        else if (c == ')') {
            pop(&st);
            lastIdx = 0;
        }
        else if (isupper((unsigned char)c)) {
            int idx;
            if (isEmpty(&st)) idx = 1; // root
            else {
                int parent = peek(&st);
                idx = (childFlag == 0) ? parent * 2 : parent * 2 + 1;
            }
            if (idx >= size) {
                fprintf(stderr, "�迭 �ʰ�!\n");
                return -1;
            }
            tree[idx] = c;
            lastIdx = idx;
            childFlag = 1; // ���� �θ� ���� ���� ���� ������
        }
    }
    return 0;
}

int bt_search(int* tree, int target, int* count_bt) {
    for (int i = 0; i < MAX_TREE; ++i) {
        (*count_bt)++;
        if (tree[i] == target) return target;
    }
}

/* -------------------- main -------------------- */
int main() {
    srand((unsigned)time(NULL));

    int nums[MAX_DATA];

    // ������ ���� 100�� ���� �� �� Ʈ���� ����
    for (int i = 0; i < MAX_DATA; i++) {
        nums[i] = rand() % 1001; // 0~1000
        bst_insert(&bst_root, nums[i]);
    }

    static int tree[MAX_TREE] = { 0 };
    parseToHeap(nums, tree, MAX_TREE);


    // ��� Ȯ��
    printf("������ ���� 100��:\n");
    for (int i = 0; i < MAX_DATA; i++) {
        printf("%d ", nums[i]);
    }
    printf("\n\n");

    int target;
    printf("ã�� ���� �Է�: ");
    scanf("%d", &target);

    int count_bst = 0, count_bt = 0;
    int bst_found = bst_search(bst_root, target, &count_bst);
    int bt_found = bt_search(tree, target, &count_bt);

    printf("\n[Ž�� ���]\n");
    if (bst_found != -1)
        printf("BST   : %d ã�� (Ž�� Ƚ��: %d)\n", target, count_bst);
    else
        printf("BST   : %d ���� (Ž�� Ƚ��: %d)\n", target, count_bst);

    if (bt_found != -1)
        printf("Binary: %d ã�� (Ž�� Ƚ��: %d, ��ġ: %d)\n", target, count_bt, bt_found);
    else
        printf("Binary: %d ���� (Ž�� Ƚ��: %d)\n", target, count_bt);

    return 0;
}
