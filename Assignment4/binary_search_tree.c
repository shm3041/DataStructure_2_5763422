#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TREE 8192  // �˳��� ���� (100�� ����)

int tree[MAX_TREE] = { 0 };

// BST ����
void insert(int key) {
    int idx = 1;
    while (idx < MAX_TREE) {
        if (tree[idx] == 0) {
            tree[idx] = key;
            return;
        }
        else if (key < tree[idx])
            idx = idx * 2;       // ����
        else if (key > tree[idx])
            idx = idx * 2 + 1;   // ������
        else
            return; // �ߺ��� ����
    }
}

// Ž�� (Ž�� Ƚ�� ����)
int search(int key, int* count) {
    int idx = 1;
    while (idx < MAX_TREE && tree[idx] != 0) {
        (*count)++;
        if (tree[idx] == key) return idx;
        else if (key < tree[idx]) idx = idx * 2;
        else idx = idx * 2 + 1;
    }
    return -1;
}

// ���� ��ȸ
void inorder(int idx) {
    if (idx >= MAX_TREE || tree[idx] == 0) return;
    inorder(idx * 2);
    printf("%d ", tree[idx]);
    inorder(idx * 2 + 1);
}

int main() {
    srand((unsigned)time(NULL));

    // 100�� ���� ����
    for (int i = 0; i < 100; i++) {
        int num = rand() % 1001;
        insert(num);
    }

    printf("���� ��ȸ ��� (��������):\n");
    inorder(1);
    printf("\n");

    int target;
    printf("\nã�� ���� �Է�: ");
    scanf("%d", &target);

    int count = 0;
    int idx = search(target, &count);

    if (idx != -1)
        printf("%d ã�� (Ž�� Ƚ��: %d, ��ġ: %d)\n", target, count, idx);
    else
        printf("%d ���� (Ž�� Ƚ��: %d)\n", target, count);

    return 0;
}
