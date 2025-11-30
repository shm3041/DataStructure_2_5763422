#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#ifndef _MSC_VER
#include <stdalign.h>
#else
/* MSVC C mode에서 alignof(max_align_t) 미지원일 수 있어 대체 */
#define alignof(t) __alignof(t)
#endif

/* =========================
   설정
   ========================= */
#define CSV_PATH "dataset_id_ascending.csv"
#define RUNS 10            /* 과제 요구: 1000 */
#define NAME_MAX 64

   /* =========================
      데이터 구조
      ========================= */
typedef struct {
    int id;
    char name[NAME_MAX];
    char gender; /* 'M' or 'F' */
    int kor, eng, math;
    int sum;
} Student;

typedef enum { CRIT_ID, CRIT_NAME, CRIT_GENDER, CRIT_SUM } Criterion;
typedef enum { ORDER_ASC, ORDER_DESC } Order;
typedef enum {
    ALG_BUBBLE,
    ALG_SELECTION,
    ALG_INSERTION,
    ALG_SHELL,
    ALG_QUICK,
    ALG_HEAP,
    ALG_MERGE,
    ALG_RADIX,
    ALG_TREE,
    ALG_COUNT
} Algorithm;

static const char* crit_name(Criterion c) {
    switch (c) {
    case CRIT_ID: return "ID";
    case CRIT_NAME: return "NAME";
    case CRIT_GENDER: return "GENDER";
    case CRIT_SUM: return "SUM(K+E+M)";
    default: return "?";
    }
}
static const char* order_name(Order o) {
    return (o == ORDER_ASC) ? "ASC" : "DESC";
}
static const char* alg_name(Algorithm a) {
    switch (a) {
    case ALG_BUBBLE: return "Bubble";
    case ALG_SELECTION: return "Selection";
    case ALG_INSERTION: return "Insertion";
    case ALG_SHELL: return "Shell";
    case ALG_QUICK: return "Quick";
    case ALG_HEAP: return "Heap";
    case ALG_MERGE: return "Merge";
    case ALG_RADIX: return "Radix";
    case ALG_TREE: return "Tree(AVL)";
    default: return "?";
    }
}

/* =========================
   측정(비교/메모리) - MSVC C 호환 버전
   - tracked_malloc/free로 "정렬 알고리즘 내부 malloc"만 추적
   ========================= */
static size_t g_mem_cur = 0;
static size_t g_mem_peak = 0;

#define TRACK_ALIGN 16u  /* 충분히 큰 정렬(대부분 환경에서 안전) */

typedef struct TrackHdr {
    void* base; /* free할 때 사용할 원본 포인터 */
    size_t size; /* 사용자가 요청한 크기(오버헤드 제외) */
} TrackHdr;

static void mem_reset(void) {
    g_mem_cur = 0;
    g_mem_peak = 0;
}

static void* tracked_malloc(size_t sz) {
    /* base + 헤더 + 정렬패딩 */
    size_t extra = (size_t)TRACK_ALIGN - 1u;
    size_t total = sizeof(TrackHdr) + extra + sz;

    unsigned char* base = (unsigned char*)malloc(total);
    if (!base) return NULL;

    uintptr_t start = (uintptr_t)(base + sizeof(TrackHdr));
    uintptr_t aligned = (start + (TRACK_ALIGN - 1u)) & ~(uintptr_t)(TRACK_ALIGN - 1u);

    TrackHdr* hdr = (TrackHdr*)(aligned - sizeof(TrackHdr));
    hdr->base = base;
    hdr->size = sz;

    g_mem_cur += sz;
    if (g_mem_cur > g_mem_peak) g_mem_peak = g_mem_cur;

    return (void*)aligned;
}

static void tracked_free(void* p) {
    if (!p) return;

    TrackHdr* hdr = (TrackHdr*)((uintptr_t)p - sizeof(TrackHdr));
    if (g_mem_cur >= hdr->size) g_mem_cur -= hdr->size;

    free(hdr->base);
}

typedef struct {
    uint64_t comparisons;
    size_t peak_extra_bytes;
} Stats;

typedef struct {
    Criterion crit;
    Order order;
    /* Radix용: 이름 최대 길이 / 점수 최대값들 */
    size_t max_name_len;
    int max_sum;
    int max_kor, max_eng, max_math;
} SortSpec;

/* =========================
   유틸
   ========================= */
static void swap_student(Student* a, Student* b) {
    Student tmp = *a;
    *a = *b;
    *b = tmp;
}

/* 비교: st->comparisons++ 포함 */
static int cmp_raw(const Student* a, const Student* b, const SortSpec* s) {
    int r = 0;
    switch (s->crit) {
    case CRIT_ID:
        r = (a->id < b->id) ? -1 : (a->id > b->id) ? 1 : 0;
        break;
    case CRIT_NAME:
        r = strcmp(a->name, b->name);
        if (r < 0) r = -1;
        else if (r > 0) r = 1;
        break;
    case CRIT_GENDER:
        r = (a->gender < b->gender) ? -1 : (a->gender > b->gender) ? 1 : 0;
        break;
    case CRIT_SUM:
        if (a->sum != b->sum) r = (a->sum < b->sum) ? -1 : 1;
        else if (a->kor != b->kor) r = (a->kor > b->kor) ? -1 : 1; /* tie: kor 큰 사람 우선 */
        else if (a->eng != b->eng) r = (a->eng > b->eng) ? -1 : 1; /* tie: eng 큰 사람 우선 */
        else if (a->math != b->math) r = (a->math > b->math) ? -1 : 1; /* tie: math 큰 사람 우선 */
        else r = 0;
        break;
    default: r = 0;
    }

    if (s->order == ORDER_DESC) r = -r;
    return r;
}
static int cmp_counted(const Student* a, const Student* b, const SortSpec* s, Stats* st) {
    st->comparisons++;
    return cmp_raw(a, b, s);
}

/* =========================
   Sorting: Bubble (stable)
   ========================= */
static void bubble_sort(Student* a, size_t n, const SortSpec* s, Stats* st) {
    if (n <= 1) return;
    size_t newn;
    do {
        newn = 0;
        for (size_t i = 1; i < n; i++) {
            if (cmp_counted(&a[i - 1], &a[i], s, st) > 0) {
                swap_student(&a[i - 1], &a[i]);
                newn = i;
            }
        }
        n = newn;
    } while (newn != 0);
}

/* =========================
   Sorting: Selection (unstable)
   ========================= */
static void selection_sort(Student* a, size_t n, const SortSpec* s, Stats* st) {
    for (size_t i = 0; i + 1 < n; i++) {
        size_t best = i;
        for (size_t j = i + 1; j < n; j++) {
            if (cmp_counted(&a[j], &a[best], s, st) < 0) best = j;
        }
        if (best != i) swap_student(&a[i], &a[best]);
    }
}

/* =========================
   Sorting: Insertion (stable)
   ========================= */
static void insertion_sort(Student* a, size_t n, const SortSpec* s, Stats* st) {
    for (size_t i = 1; i < n; i++) {
        Student key = a[i];
        size_t j = i;
        while (j > 0) {
            if (cmp_counted(&a[j - 1], &key, s, st) <= 0) break;
            a[j] = a[j - 1];
            j--;
        }
        a[j] = key;
    }
}

/* =========================
   Sorting: Shell (unstable)
   gap: n/2, /2 ...
   ========================= */
static void shell_sort(Student* a, size_t n, const SortSpec* s, Stats* st) {
    for (size_t gap = n / 2; gap > 0; gap /= 2) {
        for (size_t i = gap; i < n; i++) {
            Student temp = a[i];
            size_t j = i;
            while (j >= gap) {
                if (cmp_counted(&a[j - gap], &temp, s, st) <= 0) break;
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = temp;
        }
        if (gap == 1) break; /* size_t underflow 보호 */
    }
    /* gap==1이 마지막으로 한 번 더 돌아야 함 */
    if (n > 1) {
        size_t gap = 1;
        for (size_t i = gap; i < n; i++) {
            Student temp = a[i];
            size_t j = i;
            while (j >= gap) {
                if (cmp_counted(&a[j - gap], &temp, s, st) <= 0) break;
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = temp;
        }
    }
}

/* =========================
   Sorting: Quick (unstable) - iterative, stack O(log n)
   pivot: middle
   ========================= */
typedef struct { int l, r; } Range;

static void quick_sort(Student* a, size_t n, const SortSpec* s, Stats* st) {
    if (n <= 1) return;

    size_t depth = 0;
    while ((1ULL << depth) < (uint64_t)n) depth++;
    depth += 4;
    Range* stack = (Range*)tracked_malloc(depth * sizeof(Range));
    if (!stack) return;

    int top = 0;
    stack[top++] = (Range){ 0, (int)n - 1 };

    while (top > 0) {
        Range rg = stack[--top];
        int l = rg.l, r = rg.r;

        while (l < r) {
            int i = l, j = r;
            Student pivot = a[(l + r) / 2];

            while (i <= j) {
                while (cmp_counted(&a[i], &pivot, s, st) < 0) i++;
                while (cmp_counted(&a[j], &pivot, s, st) > 0) j--;
                if (i <= j) {
                    swap_student(&a[i], &a[j]);
                    i++; j--;
                }
            }

            /* [l..j], [i..r] */
            int left_l = l, left_r = j;
            int right_l = i, right_r = r;

            /* 작은 구간부터 처리 -> 스택 깊이 O(log n) */
            int left_size = left_r - left_l;
            int right_size = right_r - right_l;

            if (left_size < right_size) {
                if (right_l < right_r) stack[top++] = (Range){ right_l, right_r };
                r = left_r;
                l = left_l;
            }
            else {
                if (left_l < left_r) stack[top++] = (Range){ left_l, left_r };
                l = right_l;
                r = right_r;
            }

            if (top >= (int)depth) {
                /* 매우 비정상 케이스 보호 */
                break;
            }
        }
    }

    tracked_free(stack);
}

/* =========================
   Sorting: Heap (unstable)
   ========================= */
static void sift_down(Student* a, size_t start, size_t end, const SortSpec* s, Stats* st) {
    size_t root = start;
    while (1) {
        size_t child = root * 2 + 1;
        if (child > end) break;
        size_t sw = root;

        if (cmp_counted(&a[sw], &a[child], s, st) < 0) sw = child;
        if (child + 1 <= end && cmp_counted(&a[sw], &a[child + 1], s, st) < 0) sw = child + 1;

        if (sw == root) return;
        swap_student(&a[root], &a[sw]);
        root = sw;
    }
}

static void heap_sort(Student* a, size_t n, const SortSpec* s, Stats* st) {
    if (n <= 1) return;
    for (ptrdiff_t start = (ptrdiff_t)(n - 2) / 2; start >= 0; start--) {
        sift_down(a, (size_t)start, n - 1, s, st);
        if (start == 0) break;
    }
    for (size_t end = n - 1; end > 0; end--) {
        swap_student(&a[end], &a[0]);
        sift_down(a, 0, end - 1, s, st);
    }
}

/* =========================
   Sorting: Merge (stable) - bottom-up
   ========================= */
static void merge_sort(Student* a, size_t n, const SortSpec* s, Stats* st) {
    if (n <= 1) return;
    Student* tmp = (Student*)tracked_malloc(n * sizeof(Student));
    if (!tmp) return;

    for (size_t width = 1; width < n; width *= 2) {
        for (size_t i = 0; i < n; i += 2 * width) {
            size_t left = i;
            size_t mid = (i + width < n) ? (i + width) : n;
            size_t right = (i + 2 * width < n) ? (i + 2 * width) : n;

            size_t p = left, q = mid, k = left;
            while (p < mid && q < right) {
                /* stable: <= 일 때 left 먼저 */
                if (cmp_counted(&a[p], &a[q], s, st) <= 0) tmp[k++] = a[p++];
                else tmp[k++] = a[q++];
            }
            while (p < mid) tmp[k++] = a[p++];
            while (q < right) tmp[k++] = a[q++];
        }
        memcpy(a, tmp, n * sizeof(Student));
    }

    tracked_free(tmp);
}

/* =========================
   Sorting: Radix (stable)
   - CRIT_ID: 4 passes (byte)
   - CRIT_SUM: composite key 5 bytes(40bits)
   - CRIT_GENDER: 1 pass(1 char)
   - CRIT_NAME: max_name_len passes (LSD from end, short=0)
   ========================= */
static uint8_t radix_digit(const Student* x, const SortSpec* s, size_t pass) {
    if (s->crit == CRIT_ID) {
        uint32_t id = (uint32_t)x->id;
        if (s->order == ORDER_DESC) id = 0xFFFFFFFFu - id;
        return (uint8_t)((id >> (pass * 8)) & 0xFF);
    }
    if (s->crit == CRIT_SUM) {
        /* key = [p1(16bits) | p2(8) | p3(8) | p4(8)] => 40bits => 5 bytes */
        int sum = x->sum;
        int p1 = (s->order == ORDER_ASC) ? sum : (s->max_sum - sum);
        int p2 = (s->max_kor - x->kor);
        int p3 = (s->max_eng - x->eng);
        int p4 = (s->max_math - x->math);

        uint64_t key = ((uint64_t)(uint16_t)p1 << 24) |
            ((uint64_t)(uint8_t)p2 << 16) |
            ((uint64_t)(uint8_t)p3 << 8) |
            ((uint64_t)(uint8_t)p4);
        return (uint8_t)((key >> (pass * 8)) & 0xFF);
    }
    if (s->crit == CRIT_GENDER) {
        uint8_t g = (uint8_t)x->gender;
        if (s->order == ORDER_DESC) g = (uint8_t)(0xFFu - g);
        return g;
    }
    /* CRIT_NAME */
    {
        size_t len = strlen(x->name);
        uint8_t ch = 0;
        if (pass < len) {
            ch = (uint8_t)x->name[len - 1 - pass];
        }
        else {
            ch = 0; /* shorter ones come first */
        }
        if (s->order == ORDER_DESC) ch = (uint8_t)(0xFFu - ch);
        return ch;
    }
}

static size_t radix_passes(const SortSpec* s) {
    switch (s->crit) {
    case CRIT_ID: return 4;
    case CRIT_SUM: return 5; /* 40bits */
    case CRIT_GENDER: return 1;
    case CRIT_NAME: return s->max_name_len;
    default: return 0;
    }
}

static void radix_sort(Student* a, size_t n, const SortSpec* s, Stats* st) {
    (void)st; /* radix는 비교 기반이 아니라 comparisons=0 유지 */
    if (n <= 1) return;
    size_t passes = radix_passes(s);
    if (passes == 0) return;

    Student* tmp = (Student*)tracked_malloc(n * sizeof(Student));
    if (!tmp) return;

    Student* src = a;
    Student* dst = tmp;

    for (size_t p = 0; p < passes; p++) {
        size_t count[256] = { 0 };

        for (size_t i = 0; i < n; i++) {
            uint8_t d = radix_digit(&src[i], s, p);
            count[d]++;
        }
        size_t pos[256];
        size_t sum = 0;
        for (size_t i = 0; i < 256; i++) {
            pos[i] = sum;
            sum += count[i];
        }
        for (size_t i = 0; i < n; i++) {
            uint8_t d = radix_digit(&src[i], s, p);
            dst[pos[d]++] = src[i];
        }

        Student* t = src; src = dst; dst = t;
    }

    if (src != a) memcpy(a, src, n * sizeof(Student));
    tracked_free(tmp);
}

/* =========================
   Sorting: Tree Sort (AVL) - (unstable)
   - 중복 키 있으면 스킵하도록 main에서 처리
   ========================= */
typedef struct AVLNode {
    Student v;
    int h;
    struct AVLNode* l;
    struct AVLNode* r;
} AVLNode;

static int avl_height(AVLNode* n) { return n ? n->h : 0; }
static int avl_max(int a, int b) { return (a > b) ? a : b; }

static AVLNode* avl_new_node(const Student* v) {
    AVLNode* n = (AVLNode*)tracked_malloc(sizeof(AVLNode));
    if (!n) return NULL;
    n->v = *v;
    n->h = 1;
    n->l = NULL;
    n->r = NULL;
    return n;
}

static AVLNode* avl_rotate_right(AVLNode* y) {
    AVLNode* x = y->l;
    AVLNode* t2 = x->r;
    x->r = y;
    y->l = t2;
    y->h = avl_max(avl_height(y->l), avl_height(y->r)) + 1;
    x->h = avl_max(avl_height(x->l), avl_height(x->r)) + 1;
    return x;
}
static AVLNode* avl_rotate_left(AVLNode* x) {
    AVLNode* y = x->r;
    AVLNode* t2 = y->l;
    y->l = x;
    x->r = t2;
    x->h = avl_max(avl_height(x->l), avl_height(x->r)) + 1;
    y->h = avl_max(avl_height(y->l), avl_height(y->r)) + 1;
    return y;
}

static int avl_balance(AVLNode* n) {
    return n ? (avl_height(n->l) - avl_height(n->r)) : 0;
}

static AVLNode* avl_insert(AVLNode* node, const Student* v, const SortSpec* s, Stats* st) {
    if (!node) return avl_new_node(v);

    int c = cmp_counted(v, &node->v, s, st);
    if (c < 0) node->l = avl_insert(node->l, v, s, st);
    else if (c > 0) node->r = avl_insert(node->r, v, s, st);
    else {
        /* 중복 키: main에서 스킵하므로 여기 오면 그냥 오른쪽으로 */
        node->r = avl_insert(node->r, v, s, st);
    }

    node->h = 1 + avl_max(avl_height(node->l), avl_height(node->r));
    int b = avl_balance(node);

    /* LL */
    if (b > 1 && cmp_raw(v, &node->l->v, s) < 0) return avl_rotate_right(node);
    /* RR */
    if (b < -1 && cmp_raw(v, &node->r->v, s) > 0) return avl_rotate_left(node);
    /* LR */
    if (b > 1 && cmp_raw(v, &node->l->v, s) > 0) {
        node->l = avl_rotate_left(node->l);
        return avl_rotate_right(node);
    }
    /* RL */
    if (b < -1 && cmp_raw(v, &node->r->v, s) < 0) {
        node->r = avl_rotate_right(node->r);
        return avl_rotate_left(node);
    }
    return node;
}

static void avl_inorder_write(AVLNode* root, Student* out, size_t n) {
    (void)n;
    /* iterative inorder: stack */
    AVLNode** stack = (AVLNode**)tracked_malloc(n * sizeof(AVLNode*));
    if (!stack) return;

    size_t sp = 0;
    AVLNode* cur = root;
    size_t k = 0;
    while (cur || sp) {
        while (cur) {
            stack[sp++] = cur;
            cur = cur->l;
        }
        cur = stack[--sp];
        out[k++] = cur->v;
        cur = cur->r;
    }

    tracked_free(stack);
}

static void avl_free_all(AVLNode* root, size_t n) {
    if (!root) return;
    /* postorder iterative */
    AVLNode** stack = (AVLNode**)tracked_malloc(n * sizeof(AVLNode*));
    if (!stack) return;
    size_t sp = 0;
    AVLNode* cur = root;
    AVLNode* last = NULL;

    while (cur || sp) {
        if (cur) {
            stack[sp++] = cur;
            cur = cur->l;
        }
        else {
            AVLNode* peek = stack[sp - 1];
            if (peek->r && last != peek->r) {
                cur = peek->r;
            }
            else {
                tracked_free(peek);
                sp--;
                last = peek;
            }
        }
    }
    tracked_free(stack);
}

static void tree_sort_avl(Student* a, size_t n, const SortSpec* s, Stats* st) {
    if (n <= 1) return;
    AVLNode* root = NULL;
    for (size_t i = 0; i < n; i++) {
        root = avl_insert(root, &a[i], s, st);
    }
    avl_inorder_write(root, a, n);
    avl_free_all(root, n);
}

/* =========================
   중복 체크(힙/트리 스킵용)
   - CRIT_SUM은 sum 범위가 작아 배열로 체크
   - CRIT_GENDER는 2값이라 거의 항상 중복
   - CRIT_ID는 (max-min)가 5n 이하일 때 bitset-like로 체크
   - CRIT_NAME은 해시셋(간단 오픈어드레싱)
   ========================= */
static int has_dup_sum(const Student* a, size_t n, int max_sum) {
    uint8_t* seen = (uint8_t*)malloc((size_t)max_sum + 1);
    if (!seen) return 1;
    memset(seen, 0, (size_t)max_sum + 1);
    for (size_t i = 0; i < n; i++) {
        int v = a[i].sum;
        if (v < 0 || v > max_sum) continue;
        if (seen[v]) { free(seen); return 1; }
        seen[v] = 1;
    }
    free(seen);
    return 0;
}
static int has_dup_gender(const Student* a, size_t n) {
    int seenM = 0, seenF = 0;
    for (size_t i = 0; i < n; i++) {
        if (a[i].gender == 'M') {
            if (seenM) return 1;
            seenM = 1;
        }
        else if (a[i].gender == 'F') {
            if (seenF) return 1;
            seenF = 1;
        }
        else {
            /* 다른 값이 있으면 중복 판단 애매 -> 중복 있다고 처리 */
            return 1;
        }
    }
    return 0;
}
static int has_dup_id(const Student* a, size_t n) {
    int minv = a[0].id, maxv = a[0].id;
    for (size_t i = 1; i < n; i++) {
        if (a[i].id < minv) minv = a[i].id;
        if (a[i].id > maxv) maxv = a[i].id;
    }
    int64_t range = (int64_t)maxv - (int64_t)minv;
    if (range >= 0 && range <= (int64_t)(5 * n)) {
        size_t sz = (size_t)range + 1;
        uint8_t* seen = (uint8_t*)malloc(sz);
        if (!seen) return 1;
        memset(seen, 0, sz);
        for (size_t i = 0; i < n; i++) {
            size_t idx = (size_t)(a[i].id - minv);
            if (seen[idx]) { free(seen); return 1; }
            seen[idx] = 1;
        }
        free(seen);
        return 0;
    }
    /* fallback: 해시(간단) */
    size_t cap = 1;
    while (cap < n * 2) cap <<= 1;
    int* table = (int*)malloc(cap * sizeof(int));
    uint8_t* used = (uint8_t*)malloc(cap);
    if (!table || !used) { free(table); free(used); return 1; }
    memset(used, 0, cap);

    for (size_t i = 0; i < n; i++) {
        uint32_t x = (uint32_t)a[i].id;
        size_t h = (size_t)(x * 2654435761u) & (cap - 1);
        while (used[h]) {
            if (table[h] == a[i].id) { free(table); free(used); return 1; }
            h = (h + 1) & (cap - 1);
        }
        used[h] = 1;
        table[h] = a[i].id;
    }
    free(table); free(used);
    return 0;
}
static uint64_t fnv1a64(const char* s) {
    uint64_t h = 1469598103934665603ULL;
    while (*s) {
        h ^= (uint8_t)(*s++);
        h *= 1099511628211ULL;
    }
    return h;
}
static int has_dup_name(const Student* a, size_t n) {
    size_t cap = 1;
    while (cap < n * 2) cap <<= 1;
    uint64_t* hashv = (uint64_t*)malloc(cap * sizeof(uint64_t));
    int* idx = (int*)malloc(cap * sizeof(int));
    uint8_t* used = (uint8_t*)malloc(cap);
    if (!hashv || !idx || !used) {
        free(hashv); free(idx); free(used);
        return 1;
    }
    memset(used, 0, cap);

    for (size_t i = 0; i < n; i++) {
        uint64_t hv = fnv1a64(a[i].name);
        size_t h = (size_t)hv & (cap - 1);
        while (used[h]) {
            if (hashv[h] == hv) {
                if (strcmp(a[idx[h]].name, a[i].name) == 0) {
                    free(hashv); free(idx); free(used);
                    return 1;
                }
            }
            h = (h + 1) & (cap - 1);
        }
        used[h] = 1;
        hashv[h] = hv;
        idx[h] = (int)i;
    }
    free(hashv); free(idx); free(used);
    return 0;
}

static int has_duplicates_for_criterion(const Student* a, size_t n, const SortSpec* s) {
    switch (s->crit) {
    case CRIT_ID: return has_dup_id(a, n);
    case CRIT_NAME: return has_dup_name(a, n);
    case CRIT_GENDER: return has_dup_gender(a, n);
    case CRIT_SUM: return has_dup_sum(a, n, s->max_sum);
    default: return 1;
    }
}

/* =========================
   CSV 로드
   ========================= */
static int load_csv(const char* path, Student** out_arr, size_t* out_n, SortSpec* ctx_base) {
    FILE* fp = fopen(path, "r");
    if (!fp) {
        printf("ERROR: cannot open %s\n", path);
        return 0;
    }

    size_t cap = 1024;
    Student* arr = (Student*)malloc(cap * sizeof(Student));
    if (!arr) { fclose(fp); return 0; }

    char line[256];

    /* header skip */
    if (!fgets(line, (int)sizeof(line), fp)) {
        free(arr); fclose(fp); return 0;
    }

    size_t n = 0;
    ctx_base->max_name_len = 0;
    ctx_base->max_sum = 0;
    ctx_base->max_kor = ctx_base->max_eng = ctx_base->max_math = 0;

    while (fgets(line, (int)sizeof(line), fp)) {
        /* CRLF 제거 */
        size_t L = strlen(line);
        while (L > 0 && (line[L - 1] == '\n' || line[L - 1] == '\r')) line[--L] = '\0';

        char* tok = strtok(line, ",");
        if (!tok) continue;
        int id = atoi(tok);

        tok = strtok(NULL, ",");
        if (!tok) continue;
        char namebuf[NAME_MAX];
        strncpy(namebuf, tok, NAME_MAX - 1);
        namebuf[NAME_MAX - 1] = '\0';

        tok = strtok(NULL, ",");
        if (!tok) continue;
        char gender = tok[0];

        tok = strtok(NULL, ",");
        if (!tok) continue;
        int kor = atoi(tok);

        tok = strtok(NULL, ",");
        if (!tok) continue;
        int eng = atoi(tok);

        tok = strtok(NULL, ",");
        if (!tok) continue;
        int math = atoi(tok);

        if (n == cap) {
            cap *= 2;
            Student* nx = (Student*)realloc(arr, cap * sizeof(Student));
            if (!nx) { free(arr); fclose(fp); return 0; }
            arr = nx;
        }

        arr[n].id = id;
        strncpy(arr[n].name, namebuf, NAME_MAX);
        arr[n].gender = gender;
        arr[n].kor = kor;
        arr[n].eng = eng;
        arr[n].math = math;
        arr[n].sum = kor + eng + math;

        size_t nl = strlen(arr[n].name);
        if (nl > ctx_base->max_name_len) ctx_base->max_name_len = nl;
        if (arr[n].sum > ctx_base->max_sum) ctx_base->max_sum = arr[n].sum;
        if (arr[n].kor > ctx_base->max_kor) ctx_base->max_kor = arr[n].kor;
        if (arr[n].eng > ctx_base->max_eng) ctx_base->max_eng = arr[n].eng;
        if (arr[n].math > ctx_base->max_math) ctx_base->max_math = arr[n].math;

        n++;
    }

    fclose(fp);
    *out_arr = arr;
    *out_n = n;
    return 1;
}

/* =========================
   실행/측정 래퍼
   ========================= */
static int is_stable_algorithm(Algorithm alg) {
    return (alg == ALG_BUBBLE || alg == ALG_INSERTION || alg == ALG_MERGE || alg == ALG_RADIX);
}

static void run_one_algorithm(Algorithm alg, Student* work, const Student* orig, size_t n,
    const SortSpec* spec, int runs,
    double* out_avg_cmp, double* out_avg_mem)
{
    uint64_t sum_cmp = 0;
    long double sum_mem = 0.0;

    for (int t = 0; t < runs; t++) {
        memcpy(work, orig, n * sizeof(Student));

        Stats st;
        st.comparisons = 0;
        mem_reset();

        switch (alg) {
        case ALG_BUBBLE:    bubble_sort(work, n, spec, &st); break;
        case ALG_SELECTION: selection_sort(work, n, spec, &st); break;
        case ALG_INSERTION: insertion_sort(work, n, spec, &st); break;
        case ALG_SHELL:     shell_sort(work, n, spec, &st); break;
        case ALG_QUICK:     quick_sort(work, n, spec, &st); break;
        case ALG_HEAP:      heap_sort(work, n, spec, &st); break;
        case ALG_MERGE:     merge_sort(work, n, spec, &st); break;
        case ALG_RADIX:     radix_sort(work, n, spec, &st); break;
        case ALG_TREE:      tree_sort_avl(work, n, spec, &st); break;
        default: break;
        }

        st.peak_extra_bytes = g_mem_peak;
        sum_cmp += st.comparisons;
        sum_mem += (long double)st.peak_extra_bytes;
    }

    *out_avg_cmp = (runs > 0) ? (double)sum_cmp / (double)runs : 0.0;
    *out_avg_mem = (runs > 0) ? (double)(sum_mem / (long double)runs) : 0.0;
}

static void print_bytes(double b) {
    if (b < 1024.0) printf("%.0f B", b);
    else if (b < 1024.0 * 1024.0) printf("%.2f KiB", b / 1024.0);
    else printf("%.2f MiB", b / (1024.0 * 1024.0));
}

/* =========================
   main
   ========================= */
int main(void) {
    Student* orig = NULL;
    size_t n = 0;

    SortSpec base;
    if (!load_csv(CSV_PATH, &orig, &n, &base)) {
        printf("Failed to load CSV.\n");
        return 1;
    }

    printf("Loaded %zu students from %s\n", n, CSV_PATH);
    printf("RUNS = %d\n\n", RUNS);

    Student* work = (Student*)malloc(n * sizeof(Student));
    if (!work) {
        free(orig);
        printf("ERROR: out of memory\n");
        return 1;
    }

    Criterion crits[4] = { CRIT_ID, CRIT_NAME, CRIT_GENDER, CRIT_SUM };
    Order orders[2] = { ORDER_ASC, ORDER_DESC };

    for (int ci = 0; ci < 4; ci++) {
        for (int oi = 0; oi < 2; oi++) {
            SortSpec spec = base;
            spec.crit = crits[ci];
            spec.order = orders[oi];

            int dup = has_duplicates_for_criterion(orig, n, &spec);

            printf("============================================================\n");
            printf("Key: %-10s  Order: %-4s  Runs: %d\n", crit_name(spec.crit), order_name(spec.order), RUNS);
            printf("Dup in key: %s  (Heap/Tree %s)\n",
                dup ? "YES" : "NO",
                dup ? "SKIP" : "ALLOW");
            if (spec.crit == CRIT_GENDER) {
                printf("Note: GENDER requires STABLE sorts only -> unstable algorithms are SKIPPED.\n");
            }
            printf("------------------------------------------------------------\n");
            printf("%-12s | %-18s | %-18s\n", "Algorithm", "Avg Comparisons", "Avg Peak Extra Mem");
            printf("------------------------------------------------------------\n");

            for (Algorithm alg = 0; alg < ALG_COUNT; alg++) {
                int skip = 0;
                const char* reason = NULL;

                if (spec.crit == CRIT_GENDER && !is_stable_algorithm(alg)) {
                    skip = 1; reason = "not stable";
                }
                if ((alg == ALG_HEAP || alg == ALG_TREE) && dup) {
                    skip = 1; reason = "duplicate key";
                }

                if (skip) {
                    printf("%-12s | %-18s | %-18s\n", alg_name(alg), "SKIPPED", reason);
                    continue;
                }

                double avg_cmp = 0.0, avg_mem = 0.0;
                run_one_algorithm(alg, work, orig, n, &spec, RUNS, &avg_cmp, &avg_mem);

                /* radix는 비교 기반이 아니라 avg_cmp가 0이 정상 */
                printf("%-12s | %18.2f | ", alg_name(alg), avg_cmp);
                print_bytes(avg_mem);
                printf("\n");
            }

            printf("============================================================\n\n");
        }
    }

    free(work);
    free(orig);
    return 0;
}
