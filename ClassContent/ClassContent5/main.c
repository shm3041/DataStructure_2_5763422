#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%3d ", arr[i]);
    printf("\n");
}

void bubbleSort(int arr[], int n) {
    int i, j, temp;
    int compareCount = 0;
    int swapped;

    //printf("초기 배열: ");
    //printArray(arr, n);
    //printf("\n");

    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        for (j = 0; j < n - i - 1; j++) {
            compareCount++;
            if (arr[j] > arr[j + 1]) {
                // 교환
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        //printf("[%2d단계] ", i + 1);
        //printArray(arr, n);

        // 더 이상 교환이 없으면 정렬 완료
        if (!swapped) break;
    }

    printf("\n총 비교 횟수: %d\n", compareCount);
}

int main() {
    int arr[SIZE];
    srand((unsigned)time(NULL));

    // 1~100 사이 난수 20개 생성
    for (int i = 0; i < SIZE; i++)
        arr[i] = rand() % 100000 + 1;

    bubbleSort(arr, SIZE);
    return 0;
}
