#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000
#define MAX_VALUE 1000000
#define TRIALS 100

//----------------------------------------------------
// 삽입 정렬
//----------------------------------------------------
long long insertion_sort(int arr[], int n) {
    long long comparisons = 0;

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0) {
            comparisons++;               // arr[j] > key 비교
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            else break;
        }
        arr[j + 1] = key;
    }
    return comparisons;
}

//----------------------------------------------------
// 기본 Shell 정렬 (gap = n/2 → /2)
//----------------------------------------------------
long long shell_sort_basic(int arr[], int n) {
    long long comparisons = 0;

    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i - gap;

            while (j >= 0) {
                comparisons++;          // arr[j]와 temp 비교
                if (arr[j] > temp) {
                    arr[j + gap] = arr[j];
                    j -= gap;
                }
                else break;
            }
            arr[j + gap] = temp;
        }
    }
    return comparisons;
}

//----------------------------------------------------
// 최적화 Shell 정렬 (Knuth gap sequence)
// gap: 1, 4, 13, 40, 121,...
//----------------------------------------------------
long long shell_sort_knuth(int arr[], int n) {
    long long comparisons = 0;

    int gap = 1;
    while (gap < n / 3) gap = 3 * gap + 1;  // 가장 큰 gap 찾기

    for (; gap > 0; gap = (gap - 1) / 3) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i - gap;

            while (j >= 0) {
                comparisons++;          // 비교 카운트
                if (arr[j] > temp) {
                    arr[j + gap] = arr[j];
                    j -= gap;
                }
                else break;
            }
            arr[j + gap] = temp;
        }
    }
    return comparisons;
}

//----------------------------------------------------
// 난수 생성
//----------------------------------------------------
void fill_random(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % (MAX_VALUE + 1);
    }
}

int main() {
    int base[N];
    int arr_insert[N];
    int arr_shell_basic[N];
    int arr_shell_knuth[N];

    long long total_insert = 0;
    long long total_shell_basic = 0;
    long long total_shell_knuth = 0;

    srand((unsigned int)time(NULL));

    for (int t = 0; t < TRIALS; t++) {
        fill_random(base, N);

        for (int i = 0; i < N; i++) {
            arr_insert[i] = base[i];
            arr_shell_basic[i] = base[i];
            arr_shell_knuth[i] = base[i];
        }

        total_insert += insertion_sort(arr_insert, N);

        total_shell_basic += shell_sort_basic(arr_shell_basic, N);

        total_shell_knuth += shell_sort_knuth(arr_shell_knuth, N);
    }

    printf("===== 평균 비교 횟수 (%d회 테스트) =====\n", TRIALS);
    printf("삽입 정렬:            %.2f\n", (double)total_insert / TRIALS);
    printf("쉘 정렬(기본 /2):      %.2f\n", (double)total_shell_basic / TRIALS);
    printf("쉘 정렬(Knuth 최적화): %.2f\n", (double)total_shell_knuth / TRIALS);

    return 0;
}
