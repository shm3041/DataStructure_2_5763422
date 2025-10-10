#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// ��� ����ü ����
typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

// �� ��� ����
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// ����Ž��Ʈ���� ����
Node* insert(Node* root, int data) {
    if (root == NULL) return createNode(data);
    if (data < root->data)
        root->left = insert(root->left, data);
    else
        root->right = insert(root->right, data);
    return root;
}

// Ʈ������ Ž��
int searchBST(Node* root, int key, int* count) {
    (*count)++;
    if (root == NULL) return 0;
    if (root->data == key) return 1;
    if (key < root->data) return searchBST(root->left, key, count);
    else return searchBST(root->right, key, count);
}

// ����ũ���� ���� �ð� ��ȯ
double get_time_microseconds() {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1e6 / freq.QuadPart;
}

int main() {
    int arr[100];
    Node* root = NULL;
    srand((unsigned)time(NULL));

    // 0~1000 ���� ���� 100�� ���� �� �迭 ����
    for (int i = 0; i < 100; i++) {
        arr[i] = rand() % 1001;
        root = insert(root, arr[i]);  // Ʈ������ ����
		printf("%d ", arr[i]);
    }

    printf("\n");

    int target;
    printf("�˻��� ���� �Է��ϼ���: ");
    scanf("%d", &target);

    // ===== �迭 ���� Ž�� =====
    double start1 = get_time_microseconds();
    int found1 = 0;
    int linearCount = 0;
    for (int i = 0; i < 100; i++) {
        linearCount++;
        if (arr[i] == target) {
            found1 = 1;
            break;
        }
    }
    double end1 = get_time_microseconds();

    // ===== Ʈ�� Ž�� =====
    double start2 = get_time_microseconds();
    int bstCount = 0;
    int found2 = searchBST(root, target, &bstCount);
    double end2 = get_time_microseconds();

    double timeLinear = end1 - start1;
    double timeBST = end2 - start2;

    printf("\n[Ž�� ���]\n");
    printf("�迭 ���� Ž��: %s (�� Ƚ��: %d, �ҿ� �ð�: %.10f ��s)\n",
        found1 ? "ã��" : "����", linearCount, timeLinear);
    printf("����Ž��Ʈ�� Ž��: %s (�� Ƚ��: %d, �ҿ� �ð�: %.10f ��s)\n",
        found2 ? "ã��" : "����", bstCount, timeBST);

    return 0;
}
