#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

#define INITIAL_CAPACITY 1024
#define DEFAULT_RUNS 10

static void trim_token(char* s) {
    if (!s) return;

    // leading
    char* start = s;
    while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n')
        ++start;
    if (start != s) memmove(s, start, strlen(start) + 1);

    // trailing
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' ||
        s[len - 1] == '\r' || s[len - 1] == '\n')) {
        s[len - 1] = '\0';
        --len;
    }
}

// CSV 로드
// 포맷: ID,NAME,GENDER,KOREAN_GRADE,ENGLISH_GRADE,MATH_GRADE
static Student* load_students_from_csv(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int capacity = INITIAL_CAPACITY;
    int count = 0;
    Student* arr = (Student*)malloc(sizeof(Student) * (size_t)capacity);
    if (!arr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;

        char* rest = line;
        char* token = strtok(rest, ",");
        if (!token) continue;
        trim_token(token);

        char* endptr = NULL;
        long id = strtol(token, &endptr, 10);
        if (endptr == token) {
            // 헤더 라인으로 보고 무시
            continue;
        }

        if (count >= capacity) {
            capacity *= 2;
            Student* tmp = (Student*)realloc(arr, sizeof(Student) * (size_t)capacity);
            if (!tmp) {
                perror("realloc");
                free(arr);
                exit(EXIT_FAILURE);
            }
            arr = tmp;
        }

        arr[count].id = (int)id;

        // NAME
        token = strtok(NULL, ",");
        if (!token) continue;
        trim_token(token);
        strncpy(arr[count].name, token, sizeof(arr[count].name) - 1);
        arr[count].name[sizeof(arr[count].name) - 1] = '\0';

        // GENDER
        token = strtok(NULL, ",");
        if (!token) continue;
        trim_token(token);
        arr[count].gender = token[0];

        // KOREAN
        token = strtok(NULL, ",");
        if (!token) continue;
        trim_token(token);
        arr[count].korean = atoi(token);

        // ENGLISH
        token = strtok(NULL, ",");
        if (!token) continue;
        trim_token(token);
        arr[count].english = atoi(token);

        // MATH
        token = strtok(NULL, ",");
        if (!token) continue;
        trim_token(token);
        arr[count].math = atoi(token);

        arr[count].sum = arr[count].korean + arr[count].english + arr[count].math;
        ++count;
    }

    fclose(fp);
    *out_count = count;
    return arr;
}

// ===== ID 중복 검사(qsort 금지 => int용 quicksort 직접 구현) =====
static void quick_sort_int(int* a, int left, int right) {
    int i = left, j = right;
    int pivot = a[(left + right) / 2];

    while (i <= j) {
        while (a[i] < pivot) i++;
        while (a[j] > pivot) j--;
        if (i <= j) {
            int t = a[i];
            a[i] = a[j];
            a[j] = t;
            i++; j--;
        }
    }
    if (left < j) quick_sort_int(a, left, j);
    if (i < right) quick_sort_int(a, i, right);
}

static int has_duplicate_ids(const Student* arr, int n) {
    int* ids = (int*)malloc(sizeof(int) * (size_t)n);
    if (!ids) {
        perror("malloc(ids)");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; ++i) ids[i] = arr[i].id;
    quick_sort_int(ids, 0, n - 1);

    int dup = 0;
    for (int i = 1; i < n; ++i) {
        if (ids[i] == ids[i - 1]) { dup = 1; break; }
    }

    free(ids);
    return dup;
}

// ===== 알고리즘 스펙 =====
typedef void (*SortFunc)(Student*, int, SortContext*);

typedef struct {
    const char* name;
    SortFunc func;
    int is_stable;          // 안정정렬이면 GENDER 포함(use_gender=1)
    int require_unique_id;  // Tree/Heap만 1
} AlgoSpec;

int main(int argc, char* argv[]) {
    const char* filename = (argc >= 2) ? argv[1] : "students.csv";
    int runs = (argc >= 3) ? atoi(argv[2]) : DEFAULT_RUNS;
    if (runs <= 0) runs = DEFAULT_RUNS;

    int n = 0;
    Student* original = load_students_from_csv(filename, &n);
    if (n <= 0) {
        fprintf(stderr, "CSV에서 읽은 학생 데이터가 없습니다.\n");
        free(original);
        return 1;
    }

    int dup_id = has_duplicate_ids(original, n);

    printf("Loaded %d students from %s\n", n, filename);
    printf("Runs per algorithm per order: %d\n", runs);
    printf("Duplicate ID present: %s\n\n", dup_id ? "YES" : "NO");

    Student* work = (Student*)malloc(sizeof(Student) * (size_t)n);
    if (!work) {
        perror("malloc(work)");
        free(original);
        return 1;
    }

    AlgoSpec algorithms[] = {
        {"Bubble",     bubble_sort,    1, 0},
        {"Selection",  selection_sort, 0, 0},
        {"Insertion",  insertion_sort, 1, 0},
        {"Shell(/2)",  shell_sort,     0, 0},
        {"Quick(mid)", quick_sort,     0, 0},
        {"Merge",      merge_sort,     1, 0},
        {"Radix(ID)+TieFix", radix_sort, 1, 0},
        {"Tree(ID only)", tree_sort_id, 0, 1},
        {"Heap(ID only)", heap_sort_id, 0, 1},
    };
    const int NUM_ALGOS = (int)(sizeof(algorithms) / sizeof(algorithms[0]));

    SortOrder orders[2] = { ASCENDING, DESCENDING };
    const char* order_names[2] = { "Ascending", "Descending" };

    for (int ai = 0; ai < NUM_ALGOS; ++ai) {
        const AlgoSpec* spec = &algorithms[ai];

        if (spec->require_unique_id && dup_id) {
            printf("========================================\n");
            printf("Algorithm: %s\n", spec->name);
            printf("  SKIP: ID duplicates exist (assignment condition)\n\n");
            continue;
        }

        printf("========================================\n");
        printf("Algorithm: %s\n", spec->name);

        for (int oi = 0; oi < 2; ++oi) {
            long long sum_comp = 0;
            long long sum_mem_peak = 0;

            for (int run = 0; run < runs; ++run) {
                memcpy(work, original, sizeof(Student) * (size_t)n);

                SortContext ctx;
                reset_context(&ctx, orders[oi], spec->is_stable ? 1 : 0);

                spec->func(work, n, &ctx);

                sum_comp += ctx.comparisons;
                sum_mem_peak += ctx.mem_peak;
            }

            double avg_comp = (double)sum_comp / (double)runs;
            double avg_mem = (double)sum_mem_peak / (double)runs;

            printf("  Order: %s\n", order_names[oi]);
            printf("    Avg comparisons : %.2f\n", avg_comp);
            printf("    Avg extra memory: %.2f bytes (peak)\n", avg_mem);
        }

        printf("\n");
    }

    free(work);
    free(original);
    return 0;
}
