#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// 노드 구조체 정의
typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

// 새 노드 생성
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// 이진탐색트리에 삽입
Node* insert(Node* root, int data) {
    if (root == NULL) return createNode(data);
    if (data < root->data)
        root->left = insert(root->left, data);
    else
        root->right = insert(root->right, data);
    return root;
}

// 트리에서 탐색
int searchBST(Node* root, int key, int* count) {
    (*count)++;
    if (root == NULL) return 0;
    if (root->data == key) return 1;
    if (key < root->data) return searchBST(root->left, key, count);
    else return searchBST(root->right, key, count);
}

// 마이크로초 단위 시간 반환
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

    // 0~1000 사이 난수 100개 생성 및 배열 저장
    for (int i = 0; i < 100; i++) {
        arr[i] = rand() % 1001;
        root = insert(root, arr[i]);  // 트리에도 삽입
		printf("%d ", arr[i]);
    }

    printf("\n");

    int target;
    printf("검색할 값을 입력하세요: ");
    scanf("%d", &target);

    // ===== 배열 선형 탐색 =====
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

    // ===== 트리 탐색 =====
    double start2 = get_time_microseconds();
    int bstCount = 0;
    int found2 = searchBST(root, target, &bstCount);
    double end2 = get_time_microseconds();

    double timeLinear = end1 - start1;
    double timeBST = end2 - start2;

    printf("\n[탐색 결과]\n");
    printf("배열 선형 탐색: %s (비교 횟수: %d, 소요 시간: %.10f μs)\n",
        found1 ? "찾음" : "없음", linearCount, timeLinear);
    printf("이진탐색트리 탐색: %s (비교 횟수: %d, 소요 시간: %.10f μs)\n",
        found2 ? "찾음" : "없음", bstCount, timeBST);

    return 0;
}
