#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int id;
    char name[64];
    char gender;
    int korean;
    int english;
    int math;
    int product;   // 세 과목 곱
} Student;

// 전역 비교 횟수 카운터
long long sort_comparisons = 0;
long long linear_comparisons = 0;
long long binary_comparisons = 0;

// qsort에서 사용할 비교 함수 (product 기준 오름차순)
int compare_by_product(const void* a, const void* b) {
    const Student* s1 = (const Student*)a;
    const Student* s2 = (const Student*)b;

    sort_comparisons++;  // 정렬 비교 횟수 증가

    if (s1->product < s2->product) return -1;
    if (s1->product > s2->product) return 1;
    return 0;
}

// 선형 탐색 (순차 탐색)
// key: 찾고자 하는 product 값
// 반환값: 찾으면 인덱스, 못 찾으면 -1
int linear_search(Student* arr, int n, int key) {
    for (int i = 0; i < n; i++) {
        linear_comparisons++;  // key와 product 비교 1회
        if (arr[i].product == key) {
            return i;
        }
    }
    return -1;
}

// 이진 탐색 (배열은 product 기준으로 정렬되어 있어야 함)
int binary_search(Student* arr, int n, int key) {
    int left = 0;
    int right = n - 1;

    while (left <= right) {
        int mid = (left + right) / 2;

        binary_comparisons++;  // key와 product 비교 1회
        if (arr[mid].product == key) {
            return mid;
        }
        else if (key < arr[mid].product) {
            right = mid - 1;
        }
        else {
            left = mid + 1;
        }
    }
    return -1;
}

int main(void) {
    const char* filename = "dataset_id_ascending.csv";
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("파일 열기 실패");
        return 1;
    }

    // 동적 배열 (초기 capacity 1024 → 필요하면 2배씩 증가)
    int capacity = 1024;
    int count = 0;
    Student* students = (Student*)malloc(sizeof(Student) * capacity);
    if (students == NULL) {
        perror("메모리 할당 실패");
        fclose(fp);
        return 1;
    }

    char line[256];

    // 첫 줄(헤더) 건너뛰기
    if (fgets(line, sizeof(line), fp) == NULL) {
        printf("빈 파일입니다.\n");
        free(students);
        fclose(fp);
        return 1;
    }

    // 데이터 읽기
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 필요 시 capacity 증가
        if (count >= capacity) {
            capacity *= 2;
            Student* tmp = (Student*)realloc(students, sizeof(Student) * capacity);
            if (tmp == NULL) {
                perror("realloc 실패");
                free(students);
                fclose(fp);
                return 1;
            }
            students = tmp;
        }

        Student s;
        // CSV 형식: ID,NAME,GENDER,KOREAN_GRADE,ENGLISH_GRADE,MATH_GRADE
        // 예:      100001,Liam,M,89,16,66
        if (sscanf(line, "%d,%63[^,],%c,%d,%d,%d",
            &s.id, s.name, &s.gender, &s.korean, &s.english, &s.math) == 6) {
            s.product = s.korean * s.english * s.math;
            students[count++] = s;
        }
        // else: 잘못된 라인은 무시
    }

    fclose(fp);

    if (count == 0) {
        printf("학생 데이터가 없습니다.\n");
        free(students);
        return 0;
    }

    // 0 ~ 1,000,000 사이 난수 생성
    srand((unsigned int)time(NULL));
    int key = rand() % 1000001;

    printf("난수(0~1,000,000): %d\n", key);

    // -------- 1) 정렬 전 선형 탐색 --------
    int idx_linear = linear_search(students, count, key);
    if (idx_linear >= 0) {
        printf("[선형 탐색] 찾은 위치: %d (ID=%d, NAME=%s, product=%d)\n",
            idx_linear, students[idx_linear].id, students[idx_linear].name,
            students[idx_linear].product);
    }
    else {
        printf("[선형 탐색] 해당 난수를 product로 갖는 학생 없음.\n");
    }
    printf("[선형 탐색] 비교 횟수: %lld\n\n", linear_comparisons);

    // -------- 2) 정렬 + 이진 탐색 --------

    // product 기준으로 정렬
    sort_comparisons = 0;
    qsort(students, count, sizeof(Student), compare_by_product);

    // 이진 탐색 수행
    binary_comparisons = 0;
    int idx_binary = binary_search(students, count, key);
    if (idx_binary >= 0) {
        printf("[이진 탐색] 찾은 위치(정렬 후): %d (ID=%d, NAME=%s, product=%d)\n",
            idx_binary, students[idx_binary].id, students[idx_binary].name,
            students[idx_binary].product);
    }
    else {
        printf("[이진 탐색] 해당 난수를 product로 갖는 학생 없음.\n");
    }

    long long total_sort_binary = sort_comparisons + binary_comparisons;
    printf("[정렬] 비교 횟수: %lld\n", sort_comparisons);
    printf("[이진 탐색] 비교 횟수: %lld\n", binary_comparisons);
    printf("[정렬 + 이진 탐색] 비교 횟수 합: %lld\n", total_sort_binary);

    free(students);
    return 0;
}
