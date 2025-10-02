#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TREE 8192  // 넉넉히 잡음 (100개 삽입)

int tree[MAX_TREE] = { 0 };

// BST 삽입
void insert(int key) {
    int idx = 1;
    while (idx < MAX_TREE) {
        if (tree[idx] == 0) {
            tree[idx] = key;
            return;
        }
        else if (key < tree[idx])
            idx = idx * 2;       // 왼쪽
        else if (key > tree[idx])
            idx = idx * 2 + 1;   // 오른쪽
        else
            return; // 중복은 무시
    }
}

// 탐색 (탐색 횟수 포함)
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

// 중위 순회
void inorder(int idx) {
    if (idx >= MAX_TREE || tree[idx] == 0) return;
    inorder(idx * 2);
    printf("%d ", tree[idx]);
    inorder(idx * 2 + 1);
}

int main() {
    srand((unsigned)time(NULL));

    // 100개 랜덤 삽입
    for (int i = 0; i < 100; i++) {
        int num = rand() % 1001;
        insert(num);
    }

    printf("중위 순회 결과 (오름차순):\n");
    inorder(1);
    printf("\n");

    int target;
    printf("\n찾을 숫자 입력: ");
    scanf("%d", &target);

    int count = 0;
    int idx = search(target, &count);

    if (idx != -1)
        printf("%d 찾음 (탐색 횟수: %d, 위치: %d)\n", target, count, idx);
    else
        printf("%d 없음 (탐색 횟수: %d)\n", target, count);

    return 0;
}
