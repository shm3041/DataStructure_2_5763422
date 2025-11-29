#include "sort.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// =============================
// 메모리 트래킹 래퍼
// =============================
static void* sc_malloc(SortContext* ctx, size_t size) {
    void* p = malloc(size);
    if (!p) {
        fprintf(stderr, "malloc failed (size=%zu)\n", size);
        exit(EXIT_FAILURE);
    }
    if (ctx) {
        ctx->mem_current += (long long)size;
        if (ctx->mem_current > ctx->mem_peak) ctx->mem_peak = ctx->mem_current;
    }
    return p;
}

static void sc_free(SortContext* ctx, void* ptr, size_t size) {
    if (!ptr) return;
    free(ptr);
    if (ctx) {
        ctx->mem_current -= (long long)size;
        if (ctx->mem_current < 0) ctx->mem_current = 0;
    }
}

void reset_context(SortContext* ctx, SortOrder order, int use_gender) {
    if (!ctx) return;
    ctx->order = order;
    ctx->use_gender = use_gender;
    ctx->comparisons = 0;
    ctx->mem_current = 0;
    ctx->mem_peak = 0;
}

void reverse_students(Student* arr, int n) {
    for (int i = 0, j = n - 1; i < j; ++i, --j) {
        Student t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
}

// =============================
// 비교 함수들
// =============================

// ID-only 비교(항상 "오름차순 기준"으로만 비교) - Tree/Heap에서 사용
static int cmp_id_only_asc(const Student* a, const Student* b, SortContext* ctx) {
    if (ctx) ctx->comparisons++;
    if (a->id == b->id) return 0;
    return (a->id < b->id) ? -1 : 1;
}

// 복합키 비교:
// 1) ID
// 2) NAME
// 3) GENDER (ctx->use_gender == 1일 때만)
// 4) SUM
//    SUM 동점이면 4-1,4-2,4-3은 항상 "내림차순" (korean, english, math)
int cmp_student(const Student* a, const Student* b, SortContext* ctx) {
    if (ctx) ctx->comparisons++;

    // 1) ID
    if (a->id != b->id) {
        if (!ctx || ctx->order == ASCENDING) return (a->id < b->id) ? -1 : 1;
        return (a->id > b->id) ? -1 : 1;
    }

    // 2) NAME
    int nameCmp = strcmp(a->name, b->name);
    if (nameCmp != 0) {
        if (!ctx || ctx->order == ASCENDING) return nameCmp;
        return -nameCmp;
    }

    // 3) GENDER (안정 정렬일 때만 포함)
    if (ctx && ctx->use_gender && a->gender != b->gender) {
        if (ctx->order == ASCENDING) return (a->gender < b->gender) ? -1 : 1;
        return (a->gender > b->gender) ? -1 : 1;
    }

    // 4) SUM
    if (a->sum != b->sum) {
        if (!ctx || ctx->order == ASCENDING) return (a->sum < b->sum) ? -1 : 1;
        return (a->sum > b->sum) ? -1 : 1;
    }

    // SUM 동점이면 과목은 항상 내림차순
    if (a->korean != b->korean)   return (a->korean > b->korean) ? -1 : 1;
    if (a->english != b->english) return (a->english > b->english) ? -1 : 1;
    if (a->math != b->math)       return (a->math > b->math) ? -1 : 1;

    return 0;
}

// =============================
// 정렬들(복합키)
// =============================

void bubble_sort(Student* arr, int n, SortContext* ctx) {
    if (n <= 1) return;
    for (int i = 0; i < n - 1; ++i) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (cmp_student(&arr[j], &arr[j + 1], ctx) > 0) {
                Student t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

void selection_sort(Student* arr, int n, SortContext* ctx) {
    if (n <= 1) return;
    for (int i = 0; i < n - 1; ++i) {
        int best = i;
        for (int j = i + 1; j < n; ++j) {
            if (cmp_student(&arr[j], &arr[best], ctx) < 0) best = j;
        }
        if (best != i) {
            Student t = arr[i];
            arr[i] = arr[best];
            arr[best] = t;
        }
    }
}

void insertion_sort(Student* arr, int n, SortContext* ctx) {
    if (n <= 1) return;
    for (int i = 1; i < n; ++i) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= 0 && cmp_student(&arr[j], &key, ctx) > 0) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

void shell_sort(Student* arr, int n, SortContext* ctx) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            Student temp = arr[i];
            int j = i;
            while (j >= gap && cmp_student(&arr[j - gap], &temp, ctx) > 0) {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}

// Quick sort (pivot: 중앙)
static void quick_sort_rec(Student* arr, int left, int right, SortContext* ctx) {
    int i = left, j = right;
    Student pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (cmp_student(&arr[i], &pivot, ctx) < 0) i++;
        while (cmp_student(&arr[j], &pivot, ctx) > 0) j--;
        if (i <= j) {
            Student t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
            i++; j--;
        }
    }
    if (left < j)  quick_sort_rec(arr, left, j, ctx);
    if (i < right) quick_sort_rec(arr, i, right, ctx);
}

void quick_sort(Student* arr, int n, SortContext* ctx) {
    if (n <= 1) return;
    quick_sort_rec(arr, 0, n - 1, ctx);
}

// Merge sort (stable)
static void merge_sort_rec(Student* arr, Student* tmp, int left, int right, SortContext* ctx) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    merge_sort_rec(arr, tmp, left, mid, ctx);
    merge_sort_rec(arr, tmp, mid + 1, right, ctx);

    int i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        // <= : 안정성 유지(왼쪽 우선)
        if (cmp_student(&arr[i], &arr[j], ctx) <= 0) tmp[k++] = arr[i++];
        else tmp[k++] = arr[j++];
    }
    while (i <= mid) tmp[k++] = arr[i++];
    while (j <= right) tmp[k++] = arr[j++];

    for (i = left; i <= right; ++i) arr[i] = tmp[i];
}

void merge_sort(Student* arr, int n, SortContext* ctx) {
    if (n <= 1) return;
    Student* tmp = (Student*)sc_malloc(ctx, sizeof(Student) * (size_t)n);
    merge_sort_rec(arr, tmp, 0, n - 1, ctx);
    sc_free(ctx, tmp, sizeof(Student) * (size_t)n);
}

// =============================
// Radix: ID 정렬 + 동일 ID 구간만 복합키 안정 정렬
// =============================
static void insertion_sort_range(Student* arr, int left, int right, SortContext* ctx) {
    for (int i = left + 1; i <= right; ++i) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= left && cmp_student(&arr[j], &key, ctx) > 0) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

void radix_sort(Student* arr, int n, SortContext* ctx) {
    if (n <= 1) return;

    int min_id = arr[0].id;
    int max_id = arr[0].id;
    for (int i = 1; i < n; ++i) {
        if (arr[i].id < min_id) min_id = arr[i].id;
        if (arr[i].id > max_id) max_id = arr[i].id;
    }
    if (min_id < 0) {
        fprintf(stderr, "[radix_sort] negative ID not supported.\n");
        return;
    }

    Student* out = (Student*)sc_malloc(ctx, sizeof(Student) * (size_t)n);

    // LSD radix by base10 digit
    for (int exp = 1; max_id / exp > 0; exp *= 10) {
        int count[10] = { 0 };

        for (int i = 0; i < n; ++i) {
            int digit = (arr[i].id / exp) % 10;
            // 내림차순이면 digit을 뒤집어서(9-digit) 안정적으로 descending 구성
            if (ctx && ctx->order == DESCENDING) digit = 9 - digit;
            count[digit]++;
        }

        for (int i = 1; i < 10; ++i) count[i] += count[i - 1];

        for (int i = n - 1; i >= 0; --i) {
            int digit = (arr[i].id / exp) % 10;
            if (ctx && ctx->order == DESCENDING) digit = 9 - digit;
            out[--count[digit]] = arr[i];
        }

        for (int i = 0; i < n; ++i) arr[i] = out[i];
    }

    sc_free(ctx, out, sizeof(Student) * (size_t)n);

    // 동일 ID 구간은 복합키로 안정 삽입정렬(라딕스가 ID만 보므로 조건 일치용 보정)
    int start = 0;
    while (start < n) {
        int end = start;
        while (end + 1 < n && arr[end + 1].id == arr[start].id) end++;
        if (end > start) insertion_sort_range(arr, start, end, ctx);
        start = end + 1;
    }
}

// =============================
// Tree Sort (BST) : ID-only, 재귀 제거
// =============================
typedef struct TreeNode {
    Student data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

static TreeNode* tree_insert_id_iter(TreeNode* root, const Student* s, SortContext* ctx) {
    if (!root) {
        TreeNode* node = (TreeNode*)sc_malloc(ctx, sizeof(TreeNode));
        node->data = *s;
        node->left = node->right = NULL;
        return node;
    }

    TreeNode* cur = root;
    TreeNode* parent = NULL;
    int last_cmp = 0;

    while (cur) {
        parent = cur;
        last_cmp = cmp_id_only_asc(s, &cur->data, ctx);
        if (last_cmp < 0) cur = cur->left;
        else cur = cur->right; // 같으면 오른쪽(과제 조건상 중복 ID 없다고 가정)
    }

    TreeNode* node = (TreeNode*)sc_malloc(ctx, sizeof(TreeNode));
    node->data = *s;
    node->left = node->right = NULL;

    if (last_cmp < 0) parent->left = node;
    else parent->right = node;

    return root;
}

static void tree_output_iter(TreeNode* root, Student* out, int n, int reverse, SortContext* ctx) {
    TreeNode** stack = (TreeNode**)sc_malloc(ctx, sizeof(TreeNode*) * (size_t)n);
    int top = 0;
    int idx = 0;

    TreeNode* cur = root;
    while (cur || top > 0) {
        while (cur) {
            stack[top++] = cur;
            cur = reverse ? cur->right : cur->left;
        }
        cur = stack[--top];
        out[idx++] = cur->data;
        cur = reverse ? cur->left : cur->right;
    }

    sc_free(ctx, stack, sizeof(TreeNode*) * (size_t)n);
}

static void tree_free_iter(TreeNode* root, int n, SortContext* ctx) {
    if (!root) return;

    TreeNode** stack = (TreeNode**)sc_malloc(ctx, sizeof(TreeNode*) * (size_t)n);
    int top = 0;
    TreeNode* cur = root;
    TreeNode* last = NULL;

    while (cur || top > 0) {
        if (cur) {
            stack[top++] = cur;
            cur = cur->left;
        }
        else {
            TreeNode* peek = stack[top - 1];
            if (peek->right && last != peek->right) {
                cur = peek->right;
            }
            else {
                top--;
                last = peek;               // (값만) 추적용
                sc_free(ctx, peek, sizeof(TreeNode));
            }
        }
    }

    sc_free(ctx, stack, sizeof(TreeNode*) * (size_t)n);
}

void tree_sort_id(Student* arr, int n, SortContext* ctx) {
    if (n <= 1) return;

    TreeNode* root = NULL;
    for (int i = 0; i < n; ++i) root = tree_insert_id_iter(root, &arr[i], ctx);

    int reverse = (ctx && ctx->order == DESCENDING) ? 1 : 0;
    tree_output_iter(root, arr, n, reverse, ctx);
    tree_free_iter(root, n, ctx);
}

// =============================
// Heap Sort : ID-only (내림차순은 마지막 reverse)
// =============================
static void heapify_id(Student* arr, int n, int i, SortContext* ctx) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && cmp_id_only_asc(&arr[l], &arr[largest], ctx) > 0) largest = l;
    if (r < n && cmp_id_only_asc(&arr[r], &arr[largest], ctx) > 0) largest = r;

    if (largest != i) {
        Student t = arr[i];
        arr[i] = arr[largest];
        arr[largest] = t;
        heapify_id(arr, n, largest, ctx);
    }
}

void heap_sort_id(Student* arr, int n, SortContext* ctx) {
    if (n <= 1) return;

    // 항상 ID 오름차순 결과를 만들고, DESC면 reverse
    for (int i = n / 2 - 1; i >= 0; --i) heapify_id(arr, n, i, ctx);
    for (int i = n - 1; i > 0; --i) {
        Student t = arr[0];
        arr[0] = arr[i];
        arr[i] = t;
        heapify_id(arr, i, 0, ctx);
    }

    if (ctx && ctx->order == DESCENDING) reverse_students(arr, n);
}
