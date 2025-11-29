#ifndef SORT_H
#define SORT_H

#include <stddef.h>

// 학생 정보 구조체
typedef struct {
    int  id;               // 학생 ID
    char name[64];         // 이름
    char gender;           // 성별 (예: 'M', 'F')
    int  korean;           // 국어
    int  english;          // 영어
    int  math;             // 수학
    int  sum;              // 합계 (korean+english+math)
} Student;

// 정렬 방향
typedef enum {
    ASCENDING = 0,
    DESCENDING = 1
} SortOrder;

// 정렬 통계 + 옵션
typedef struct {
    SortOrder order;       // ASCENDING / DESCENDING
    int use_gender;        // 1이면 GENDER를 복합키에 포함, 0이면 무시

    long long comparisons; // 비교 횟수(비교 함수 호출 횟수)
    long long mem_current; // 현재 추가 동적 메모리 사용량(bytes)
    long long mem_peak;    // 정렬 중 최대 추가 동적 메모리 사용량(bytes)
} SortContext;

void reset_context(SortContext* ctx, SortOrder order, int use_gender);

// 복합키 비교(ID->NAME->(GENDER)->SUM->K/E/M(desc))
int cmp_student(const Student* a, const Student* b, SortContext* ctx);

// 정렬 함수들
void bubble_sort(Student* arr, int n, SortContext* ctx);
void selection_sort(Student* arr, int n, SortContext* ctx);
void insertion_sort(Student* arr, int n, SortContext* ctx);
void shell_sort(Student* arr, int n, SortContext* ctx);
void quick_sort(Student* arr, int n, SortContext* ctx);
void merge_sort(Student* arr, int n, SortContext* ctx);

// Radix: ID(정수)로 먼저 정렬한 뒤, 동일 ID 구간은 복합키로 안정 정렬(조건 일치용)
void radix_sort(Student* arr, int n, SortContext* ctx);

// Tree/Heap: 과제 조건대로 ID만 기준으로 정렬(중복 ID 없을 때만 적용)
void tree_sort_id(Student* arr, int n, SortContext* ctx);
void heap_sort_id(Student* arr, int n, SortContext* ctx);

void reverse_students(Student* arr, int n);

#endif // SORT_H
