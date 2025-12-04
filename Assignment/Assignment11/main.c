#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
} Student;

/* =========================
   CSV 로딩
   ========================= */
Student* load_students(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    char line[MAX_LINE_LEN];
    int capacity = 10;
    int count = 0;
    Student* arr = (Student*)malloc(sizeof(Student) * capacity);

    if (!arr) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    // 헤더 스킵
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        free(arr);
        *out_count = 0;
        return NULL;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* temp = (Student*)realloc(arr, sizeof(Student) * capacity);
            if (!temp) {
                perror("Reallocation failed");
                free(arr);
                fclose(fp);
                return NULL;
            }
            arr = temp;
        }

        Student s;
        char* token = strtok(line, ",");
        if (!token) continue;
        s.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(s.name, token, MAX_NAME_LEN - 1);
        s.name[MAX_NAME_LEN - 1] = '\0';
        char* nl = strchr(s.name, '\n');
        if (nl) *nl = '\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        s.gender = token[0];

        token = strtok(NULL, ",");
        if (!token) continue;
        s.korean = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        s.english = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        s.math = atoi(token);

        arr[count++] = s;
    }

    fclose(fp);

    Student* tight = (Student*)realloc(arr, sizeof(Student) * count);
    if (!tight) {
        fprintf(stderr, "Warning: Tight reallocation failed, using original memory.\n");
        *out_count = count;
        return arr;
    }

    *out_count = count;
    return tight;
}

/* =========================
   키 타입 / 비교 함수
   ========================= */
typedef enum {
    KEY_ID = 0,
    KEY_NAME,
    KEY_GENDER,
    KEY_KOREAN,
    KEY_ENGLISH,
    KEY_MATH,
    KEY_COUNT
} KeyType;

const char* KEY_NAME_STR[KEY_COUNT] = {
    "ID", "NAME", "GENDER", "KOREAN", "ENGLISH", "MATH"
};

/* 비교 횟수 카운터 */
static long long* g_cmp_counter = NULL;

static inline void add_cmp(void) {
    if (g_cmp_counter) (*g_cmp_counter)++;
}

/* Student vs Student (키 기준) */
int compare_student_by_key(const Student* a, const Student* b, KeyType key) {
    add_cmp();
    switch (key) {
    case KEY_ID:
        if (a->id < b->id) return -1;
        if (a->id > b->id) return 1;
        return 0;
    case KEY_NAME:
        return strcmp(a->name, b->name);
    case KEY_GENDER:
        if (a->gender < b->gender) return -1;
        if (a->gender > b->gender) return 1;
        return 0;
    case KEY_KOREAN:
        if (a->korean < b->korean) return -1;
        if (a->korean > b->korean) return 1;
        return 0;
    case KEY_ENGLISH:
        if (a->english < b->english) return -1;
        if (a->english > b->english) return 1;
        return 0;
    case KEY_MATH:
        if (a->math < b->math) return -1;
        if (a->math > b->math) return 1;
        return 0;
    default:
        return 0;
    }
}

/* Student vs keyVal (순차, 이진 탐색용) */
int compare_student_to_key(const Student* s, const void* keyVal, KeyType key) {
    add_cmp();
    switch (key) {
    case KEY_ID: {
        int v = *(const int*)keyVal;
        if (s->id < v) return -1;
        if (s->id > v) return 1;
        return 0;
    }
    case KEY_NAME: {
        const char* str = (const char*)keyVal;
        return strcmp(s->name, str);
    }
    case KEY_GENDER: {
        char g = *(const char*)keyVal;
        if (s->gender < g) return -1;
        if (s->gender > g) return 1;
        return 0;
    }
    case KEY_KOREAN: {
        int v = *(const int*)keyVal;
        if (s->korean < v) return -1;
        if (s->korean > v) return 1;
        return 0;
    }
    case KEY_ENGLISH: {
        int v = *(const int*)keyVal;
        if (s->english < v) return -1;
        if (s->english > v) return 1;
        return 0;
    }
    case KEY_MATH: {
        int v = *(const int*)keyVal;
        if (s->math < v) return -1;
        if (s->math > v) return 1;
        return 0;
    }
    default:
        return 0;
    }
}

/* keyVal vs Student (AVL 탐색/삭제용) : keyVal < s 이면 -1 */
int compare_key_to_student(const void* keyVal, const Student* s, KeyType key) {
    add_cmp();
    switch (key) {
    case KEY_ID: {
        int v = *(const int*)keyVal;
        if (v < s->id) return -1;
        if (v > s->id) return 1;
        return 0;
    }
    case KEY_NAME: {
        const char* str = (const char*)keyVal;
        int c = strcmp(str, s->name);
        if (c < 0) return -1;
        if (c > 0) return 1;
        return 0;
    }
    case KEY_GENDER: {
        char g = *(const char*)keyVal;
        if (g < s->gender) return -1;
        if (g > s->gender) return 1;
        return 0;
    }
    case KEY_KOREAN: {
        int v = *(const int*)keyVal;
        if (v < s->korean) return -1;
        if (v > s->korean) return 1;
        return 0;
    }
    case KEY_ENGLISH: {
        int v = *(const int*)keyVal;
        if (v < s->english) return -1;
        if (v > s->english) return 1;
        return 0;
    }
    case KEY_MATH: {
        int v = *(const int*)keyVal;
        if (v < s->math) return -1;
        if (v > s->math) return 1;
        return 0;
    }
    default:
        return 0;
    }
}

/* =========================
   비정렬 배열
   ========================= */
typedef struct {
    Student* data;
    int size;
    int capacity;
} UnsortedArray;

UnsortedArray* ua_create_from_base(const Student* base, int n) {
    UnsortedArray* ua = (UnsortedArray*)malloc(sizeof(UnsortedArray));
    ua->capacity = n + 100;
    ua->size = n;
    ua->data = (Student*)malloc(sizeof(Student) * ua->capacity);
    memcpy(ua->data, base, sizeof(Student) * n);
    return ua;
}

void ua_destroy(UnsortedArray* ua) {
    if (!ua) return;
    free(ua->data);
    free(ua);
}

/* 순차 탐색 (검색/삭제용) */
int ua_seq_search(UnsortedArray* ua, KeyType key, const void* keyVal, long long* cmp_count) {
    g_cmp_counter = cmp_count;
    if (cmp_count) *cmp_count = 0;
    for (int i = 0; i < ua->size; ++i) {
        if (compare_student_to_key(&ua->data[i], keyVal, key) == 0)
            return i;
    }
    return -1;
}

/* 삽입: 뒤에 그냥 붙이기 (비교 없음) */
void ua_insert(UnsortedArray* ua, const Student* s, long long* cmp_count) {
    g_cmp_counter = cmp_count;
    if (cmp_count) *cmp_count = 0;
    if (ua->size >= ua->capacity) {
        ua->capacity *= 2;
        ua->data = (Student*)realloc(ua->data, sizeof(Student) * ua->capacity);
    }
    ua->data[ua->size++] = *s;
}

/* 삭제: 찾은 뒤 마지막 원소로 덮기 */
int ua_delete(UnsortedArray* ua, KeyType key, const void* keyVal, long long* cmp_count) {
    int idx = ua_seq_search(ua, key, keyVal, cmp_count);
    if (idx == -1) return 0;
    ua->data[idx] = ua->data[ua->size - 1];
    ua->size--;
    return 1;
}

/* =========================
   정렬 배열 (병합 정렬 + 이진 탐색)
   ========================= */
typedef struct {
    Student* data;
    int size;
    int capacity;
    KeyType key;
} SortedArray;

/* 병합 정렬 (비교 횟수 측정 안 함) */
static void merge(Student* arr, Student* tmp, int left, int mid, int right, KeyType key) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (compare_student_by_key(&arr[i], &arr[j], key) <= 0) {
            tmp[k++] = arr[i++];
        }
        else {
            tmp[k++] = arr[j++];
        }
    }
    while (i <= mid) tmp[k++] = arr[i++];
    while (j <= right) tmp[k++] = arr[j++];
    for (i = left; i <= right; ++i) arr[i] = tmp[i];
}

static void merge_sort_rec(Student* arr, Student* tmp, int left, int right, KeyType key) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    merge_sort_rec(arr, tmp, left, mid, key);
    merge_sort_rec(arr, tmp, mid + 1, right, key);
    merge(arr, tmp, left, mid, right, key);
}

void sort_students(Student* arr, int n, KeyType key) {
    g_cmp_counter = NULL; // 정렬 중 비교는 카운트하지 않음
    Student* tmp = (Student*)malloc(sizeof(Student) * n);
    if (!tmp) return;
    merge_sort_rec(arr, tmp, 0, n - 1, key);
    free(tmp);
}

SortedArray* sa_create_from_base(const Student* base, int n, KeyType key) {
    SortedArray* sa = (SortedArray*)malloc(sizeof(SortedArray));
    sa->capacity = n + 100;
    sa->size = n;
    sa->key = key;
    sa->data = (Student*)malloc(sizeof(Student) * sa->capacity);
    memcpy(sa->data, base, sizeof(Student) * n);
    sort_students(sa->data, sa->size, key);
    return sa;
}

void sa_destroy(SortedArray* sa) {
    if (!sa) return;
    free(sa->data);
    free(sa);
}

/* 이진 탐색 */
int sa_binary_search(SortedArray* sa, KeyType key, const void* keyVal, long long* cmp_count) {
    g_cmp_counter = cmp_count;
    if (cmp_count) *cmp_count = 0;

    int left = 0, right = sa->size - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = compare_student_to_key(&sa->data[mid], keyVal, key);
        if (cmp == 0) return mid;
        else if (cmp < 0) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

/* 정렬 유지 삽입: lower_bound 위치에 끼워넣기 */
void sa_insert(SortedArray* sa, const Student* s, long long* cmp_count) {
    g_cmp_counter = cmp_count;
    if (cmp_count) *cmp_count = 0;

    if (sa->size >= sa->capacity) {
        sa->capacity *= 2;
        sa->data = (Student*)realloc(sa->data, sizeof(Student) * sa->capacity);
    }

    int left = 0, right = sa->size;
    while (left < right) {
        int mid = (left + right) / 2;
        int cmp = compare_student_by_key(s, &sa->data[mid], sa->key);
        if (cmp > 0) left = mid + 1;
        else right = mid;
    }
    int pos = left;

    memmove(&sa->data[pos + 1], &sa->data[pos],
        sizeof(Student) * (sa->size - pos));
    sa->data[pos] = *s;
    sa->size++;
}

/* 삭제: 이진 탐색 후 쉬프트 */
int sa_delete(SortedArray* sa, KeyType key, const void* keyVal, long long* cmp_count) {
    int idx = sa_binary_search(sa, key, keyVal, cmp_count);
    if (idx == -1) return 0;
    memmove(&sa->data[idx], &sa->data[idx + 1],
        sizeof(Student) * (sa->size - idx - 1));
    sa->size--;
    return 1;
}

/* =========================
   AVL 트리
   ========================= */
typedef struct AVLNode {
    Student data;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

int avl_height(AVLNode* n) { return n ? n->height : 0; }
int avl_max(int a, int b) { return (a > b) ? a : b; }

AVLNode* avl_new_node(const Student* s) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->data = *s;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

/* 회전은 안전장치 달아둠 (논리적으로는 호출 시 left/right NULL이면 안 됨) */
AVLNode* avl_right_rotate(AVLNode* y) {
    if (!y || !y->left) return y; // 안전장치
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    return x;
}

AVLNode* avl_left_rotate(AVLNode* x) {
    if (!x || !x->right) return x; // 안전장치
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    return y;
}

int avl_get_balance(AVLNode* n) {
    if (!n) return 0;
    return avl_height(n->left) - avl_height(n->right);
}

/* AVL 삽입: Student 전체를 값으로 사용, key 기준으로 정렬 */
AVLNode* avl_insert(AVLNode* node, const Student* s, KeyType key, long long* cmp_count) {
    g_cmp_counter = cmp_count;

    if (node == NULL)
        return avl_new_node(s);

    // s vs node->data (s < node -> 왼쪽)
    int cmp = compare_student_by_key(s, &node->data, key);

    if (cmp < 0)
        node->left = avl_insert(node->left, s, key, cmp_count);
    else if (cmp > 0)
        node->right = avl_insert(node->right, s, key, cmp_count);
    else
        node->right = avl_insert(node->right, s, key, cmp_count); // 중복은 오른쪽으로

    node->height = 1 + avl_max(avl_height(node->left), avl_height(node->right));
    int balance = avl_get_balance(node);

    // LL
    if (balance > 1 && compare_student_by_key(s, &node->left->data, key) < 0)
        return avl_right_rotate(node);

    // RR
    if (balance < -1 && compare_student_by_key(s, &node->right->data, key) > 0)
        return avl_left_rotate(node);

    // LR
    if (balance > 1 && compare_student_by_key(s, &node->left->data, key) > 0) {
        node->left = avl_left_rotate(node->left);
        return avl_right_rotate(node);
    }

    // RL
    if (balance < -1 && compare_student_by_key(s, &node->right->data, key) < 0) {
        node->right = avl_right_rotate(node->right);
        return avl_left_rotate(node);
    }

    return node;
}

AVLNode* avl_min_value_node(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left)
        current = current->left;
    return current;
}

/* AVL 검색: keyVal로 탐색 */
AVLNode* avl_search(AVLNode* root, KeyType key, const void* keyVal, long long* cmp_count) {
    g_cmp_counter = cmp_count;
    if (cmp_count) *cmp_count = 0;

    AVLNode* cur = root;
    while (cur) {
        int cmp = compare_key_to_student(keyVal, &cur->data, key);
        if (cmp == 0) return cur;
        else if (cmp < 0) cur = cur->left;
        else cur = cur->right;
    }
    return NULL;
}

/* AVL 삭제: keyVal 기준 삭제 */
AVLNode* avl_delete(AVLNode* root, KeyType key, const void* keyVal, long long* cmp_count) {
    g_cmp_counter = cmp_count;

    if (!root) return root;

    int cmp = compare_key_to_student(keyVal, &root->data, key);
    if (cmp < 0) {
        root->left = avl_delete(root->left, key, keyVal, cmp_count);
    }
    else if (cmp > 0) {
        root->right = avl_delete(root->right, key, keyVal, cmp_count);
    }
    else {
        // 해당 노드 발견
        if (!root->left || !root->right) {
            AVLNode* temp = root->left ? root->left : root->right;
            if (!temp) {
                temp = root;
                root = NULL;
            }
            else {
                *root = *temp;
            }
            free(temp);
        }
        else {
            // 오른쪽 서브트리에서 최솟값으로 교체
            AVLNode* temp = avl_min_value_node(root->right);
            root->data = temp->data;

            int key_int; char key_char; char key_str[MAX_NAME_LEN];
            const void* keyVal2 = NULL;
            switch (key) {
            case KEY_ID:
                key_int = temp->data.id; keyVal2 = &key_int; break;
            case KEY_NAME:
                strcpy(key_str, temp->data.name); keyVal2 = key_str; break;
            case KEY_GENDER:
                key_char = temp->data.gender; keyVal2 = &key_char; break;
            case KEY_KOREAN:
                key_int = temp->data.korean; keyVal2 = &key_int; break;
            case KEY_ENGLISH:
                key_int = temp->data.english; keyVal2 = &key_int; break;
            case KEY_MATH:
                key_int = temp->data.math; keyVal2 = &key_int; break;
            default:
                keyVal2 = &key_int; break;
            }
            root->right = avl_delete(root->right, key, keyVal2, cmp_count);
        }
    }

    if (!root) return root;

    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));
    int balance = avl_get_balance(root);

    // LL
    if (balance > 1 && avl_get_balance(root->left) >= 0)
        return avl_right_rotate(root);

    // LR
    if (balance > 1 && avl_get_balance(root->left) < 0) {
        root->left = avl_left_rotate(root->left);
        return avl_right_rotate(root);
    }

    // RR
    if (balance < -1 && avl_get_balance(root->right) <= 0)
        return avl_left_rotate(root);

    // RL
    if (balance < -1 && avl_get_balance(root->right) > 0) {
        root->right = avl_right_rotate(root->right);
        return avl_left_rotate(root);
    }

    return root;
}

void avl_free(AVLNode* root) {
    if (!root) return;
    avl_free(root->left);
    avl_free(root->right);
    free(root);
}

/* =========================
   키 값 추출
   ========================= */
void extract_key_value(const Student* s, KeyType key, void* out) {
    switch (key) {
    case KEY_ID:
        *(int*)out = s->id; break;
    case KEY_NAME:
        strcpy((char*)out, s->name); break;
    case KEY_GENDER:
        *(char*)out = s->gender; break;
    case KEY_KOREAN:
        *(int*)out = s->korean; break;
    case KEY_ENGLISH:
        *(int*)out = s->english; break;
    case KEY_MATH:
        *(int*)out = s->math; break;
    default:
        break;
    }
}

/* =========================
   메인: 6개 키 × 3연산 × 3구조 × 10회
   ========================= */
int main(void) {
    const char* filename = "dataset_id_ascending.csv"; // 네 CSV 이름으로 수정 가능
    int n = 0;
    Student* base = load_students(filename, &n);
    if (!base) {
        fprintf(stderr, "Failed to load students\n");
        return 1;
    }
    printf("Loaded %d students from %s\n\n", n, filename);

    srand((unsigned)time(NULL));

    const int TRIALS = 10;
    // [자료구조][연산] 평균 비교 횟수
    // ds: 0 = Unsorted, 1 = Sorted, 2 = AVL
    // op: 0 = Insert, 1 = Delete, 2 = Search
    double avg_cmp[3][3];

    for (int k = 0; k < KEY_COUNT; ++k) {
        KeyType key = (KeyType)k;
        printf("============================================\n");
        printf("Key: %s\n", KEY_NAME_STR[k]);
        printf("============================================\n");

        for (int ds = 0; ds < 3; ++ds)
            for (int op = 0; op < 3; ++op)
                avg_cmp[ds][op] = 0.0;

        // Insert / Delete / Search 각각 10회
        for (int op = 0; op < 3; ++op) {
            for (int t = 0; t < TRIALS; ++t) {
                int idx = rand() % n;
                const Student* target = &base[idx];

                int key_int; char key_char; char key_str[MAX_NAME_LEN];
                void* keyValPtr = NULL;
                switch (key) {
                case KEY_ID:
                case KEY_KOREAN:
                case KEY_ENGLISH:
                case KEY_MATH:
                    extract_key_value(target, key, &key_int);
                    keyValPtr = &key_int;
                    break;
                case KEY_NAME:
                    extract_key_value(target, key, key_str);
                    keyValPtr = key_str;
                    break;
                case KEY_GENDER:
                    extract_key_value(target, key, &key_char);
                    keyValPtr = &key_char;
                    break;
                default:
                    break;
                }

                long long cmp_unsorted = 0, cmp_sorted = 0, cmp_avl = 0;

                /* 비정렬 배열 */
                UnsortedArray* ua = ua_create_from_base(base, n);
                if (op == 0) {            // Insert
                    ua_insert(ua, target, &cmp_unsorted);
                }
                else if (op == 1) {     // Delete
                    ua_delete(ua, key, keyValPtr, &cmp_unsorted);
                }
                else {                  // Search
                    ua_seq_search(ua, key, keyValPtr, &cmp_unsorted);
                }
                ua_destroy(ua);

                /* 정렬 배열 */
                SortedArray* sa = sa_create_from_base(base, n, key);
                if (op == 0) {            // Insert
                    sa_insert(sa, target, &cmp_sorted);
                }
                else if (op == 1) {     // Delete
                    sa_delete(sa, key, keyValPtr, &cmp_sorted);
                }
                else {                  // Search
                    sa_binary_search(sa, key, keyValPtr, &cmp_sorted);
                }
                sa_destroy(sa);

                /* AVL 트리 */
                AVLNode* root = NULL;
                // 빌드할 때는 비교횟수 측정 안 함
                for (int i = 0; i < n; ++i) {
                    root = avl_insert(root, &base[i], key, NULL);
                }

                if (op == 0) {            // Insert
                    root = avl_insert(root, target, key, &cmp_avl);
                }
                else if (op == 1) {     // Delete
                    root = avl_delete(root, key, keyValPtr, &cmp_avl);
                }
                else {                  // Search
                    avl_search(root, key, keyValPtr, &cmp_avl);
                }

                avl_free(root);

                avg_cmp[0][op] += (double)cmp_unsorted;
                avg_cmp[1][op] += (double)cmp_sorted;
                avg_cmp[2][op] += (double)cmp_avl;
            }

            for (int ds = 0; ds < 3; ++ds)
                avg_cmp[ds][op] /= TRIALS;
        }

        const char* ds_name[3] = {
            "Unsorted Array (Seq)",
            "Sorted Array (Binary)",
            "AVL Tree"
        };
        const char* op_name[3] = { "Insert", "Delete", "Search" };

        for (int ds = 0; ds < 3; ++ds) {
            printf("---- %s ----\n", ds_name[ds]);
            for (int op = 0; op < 3; ++op) {
                printf("  %-6s : Avg Comparisons = %.2f\n",
                    op_name[op], avg_cmp[ds][op]);
            }
        }
        printf("\n");
    }

    free(base);
    return 0;
}
