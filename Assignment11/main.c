#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STUDENTS 40000
#define MAX_NAME_LEN 32
#define TRIALS 40

// 학생 구조체
typedef struct {
    int  id;
    char name[MAX_NAME_LEN];
    char gender;
    int  kor, eng, math;
} Student;

// AVL 노드
typedef struct AVLNode {
    Student data;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

// 연산(op) 정보
typedef struct {
    int key; // ID
    int op;  // 0 = insert, 1 = delete, 2 = search
} Operation;

// ===== 유틸 =====
static int max_int(int a, int b) { return (a > b) ? a : b; }
static int height(AVLNode* n) { return n ? n->height : 0; }

static int get_balance(AVLNode* n) {
    return n ? height(n->left) - height(n->right) : 0;
}

static AVLNode* new_avl_node(Student s) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        perror("malloc");
        exit(1);
    }
    node->data = s;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

static AVLNode* right_rotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max_int(height(y->left), height(y->right)) + 1;
    x->height = max_int(height(x->left), height(x->right)) + 1;

    return x;
}

static AVLNode* left_rotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max_int(height(x->left), height(x->right)) + 1;
    y->height = max_int(height(y->left), height(y->right)) + 1;

    return y;
}

// ===== CSV 로딩 (ID,NAME,GENDER,KOR,ENG,MATH 가정) =====
int load_students(const char* filename, Student* arr, int max_n) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    char line[256];
    int n = 0;

    // 헤더 스킵 (있다고 가정)
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (n >= max_n) break;

        char* p = strchr(line, '\n');
        if (p) *p = '\0';

        Student s;
        memset(&s, 0, sizeof(s));

        char* tok = strtok(line, ",");
        if (!tok) continue;
        s.id = atoi(tok);

        tok = strtok(NULL, ",");
        if (!tok) continue;
        strncpy(s.name, tok, MAX_NAME_LEN - 1);
        s.name[MAX_NAME_LEN - 1] = '\0';

        tok = strtok(NULL, ",");
        if (!tok) continue;
        s.gender = tok[0];

        tok = strtok(NULL, ",");
        s.kor = tok ? atoi(tok) : 0;
        tok = strtok(NULL, ",");
        s.eng = tok ? atoi(tok) : 0;
        tok = strtok(NULL, ",");
        s.math = tok ? atoi(tok) : 0;

        arr[n++] = s;
    }

    fclose(fp);
    return n;
}

// ===== 삽입 정렬 (ID 기준 오름차순) =====
void insertion_sort_by_id(Student* arr, int n) {
    int i, j;
    for (i = 1; i < n; ++i) {
        Student key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j].id > key.id) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// ===== 순차 탐색 (비정렬 배열 검색) =====
int seq_search_unsorted(Student* arr, int n, int key, long long* comp) {
    int i;
    for (i = 0; i < n; ++i) {
        (*comp)++;
        if (arr[i].id == key) return i;
    }
    return -1;
}

// ===== 이진 탐색 (정렬 배열 검색) =====
int binary_search_pos(Student* arr, int n, int key, long long* comp, int* found) {
    int left = 0, right = n - 1, mid;
    if (found) *found = 0;

    while (left <= right) {
        mid = (left + right) / 2;
        (*comp)++; // == 비교
        if (key == arr[mid].id) {
            if (found) *found = 1;
            return mid;
        }
        (*comp)++; // < 비교
        if (key < arr[mid].id) {
            right = mid - 1;
        }
        else {
            left = mid + 1;
        }
    }
    return left; // 삽입 위치
}

int binary_search_sorted(Student* arr, int n, int key, long long* comp) {
    int found;
    int pos = binary_search_pos(arr, n, key, comp, &found);
    return found ? pos : -1;
}

// ===== 비정렬 배열 삽입/삭제 =====
int insert_unsorted(Student* arr, int* pn, int capacity, int key, long long* comp) {
    int n = *pn;
    int idx = seq_search_unsorted(arr, n, key, comp);
    if (idx != -1) return 0;            // 이미 존재
    if (n >= capacity) return 0;        // 공간 부족

    Student s;
    memset(&s, 0, sizeof(s));
    s.id = key;
    arr[n] = s;
    *pn = n + 1;
    return 1;
}

int delete_unsorted(Student* arr, int* pn, int key, long long* comp) {
    int n = *pn;
    int idx = seq_search_unsorted(arr, n, key, comp);
    if (idx == -1) return 0;            // 못 찾음
    arr[idx] = arr[n - 1];
    *pn = n - 1;
    return 1;
}

// ===== 정렬 배열 삽입/삭제 =====
int insert_sorted(Student* arr, int* pn, int capacity, int key, long long* comp) {
    int n = *pn;
    int found;
    int pos = binary_search_pos(arr, n, key, comp, &found);
    if (found) return 0;                // 이미 존재
    if (n >= capacity) return 0;

    int i;
    for (i = n; i > pos; --i)
        arr[i] = arr[i - 1];

    Student s;
    memset(&s, 0, sizeof(s));
    s.id = key;
    arr[pos] = s;
    *pn = n + 1;
    return 1;
}

int delete_sorted(Student* arr, int* pn, int key, long long* comp) {
    int n = *pn;
    int found;
    int pos = binary_search_pos(arr, n, key, comp, &found);
    if (!found) return 0;

    int i;
    for (i = pos; i < n - 1; ++i)
        arr[i] = arr[i + 1];

    *pn = n - 1;
    return 1;
}

// ===== AVL 삽입/검색/삭제 =====
AVLNode* avl_insert(AVLNode* node, Student s, long long* comp, int* success) {
    if (!node) {
        *success = 1;
        return new_avl_node(s);
    }

    int key = s.id;

    (*comp)++; // < 비교
    if (key < node->data.id) {
        node->left = avl_insert(node->left, s, comp, success);
    }
    else {
        (*comp)++; // > 비교
        if (key > node->data.id) {
            node->right = avl_insert(node->right, s, comp, success);
        }
        else {
            *success = 0; // 중복
            return node;
        }
    }

    node->height = 1 + max_int(height(node->left), height(node->right));
    int balance = get_balance(node);

    // LL
    if (balance > 1 && key < node->left->data.id)
        return right_rotate(node);
    // RR
    if (balance < -1 && key > node->right->data.id)
        return left_rotate(node);
    // LR
    if (balance > 1 && key > node->left->data.id) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    // RL
    if (balance < -1 && key < node->right->data.id) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

AVLNode* avl_search(AVLNode* node, int key, long long* comp) {
    while (node) {
        (*comp)++; // == 비교
        if (key == node->data.id) return node;
        (*comp)++; // < 비교
        if (key < node->data.id) node = node->left;
        else node = node->right;
    }
    return NULL;
}

AVLNode* min_value_node(AVLNode* node) {
    AVLNode* cur = node;
    while (cur && cur->left)
        cur = cur->left;
    return cur;
}

AVLNode* avl_delete(AVLNode* root, int key, long long* comp, int* success) {
    if (!root) return root;

    (*comp)++; // < 비교
    if (key < root->data.id) {
        root->left = avl_delete(root->left, key, comp, success);
    }
    else {
        (*comp)++; // > 비교
        if (key > root->data.id) {
            root->right = avl_delete(root->right, key, comp, success);
        }
        else {
            // 삭제 대상 찾음
            *success = 1;
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
                AVLNode* temp = min_value_node(root->right);
                root->data = temp->data;
                root->right = avl_delete(root->right, temp->data.id, comp, success);
            }
        }
    }

    if (!root) return root;

    root->height = 1 + max_int(height(root->left), height(root->right));
    int balance = get_balance(root);

    // LL
    if (balance > 1 && get_balance(root->left) >= 0)
        return right_rotate(root);
    // LR
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }
    // RR
    if (balance < -1 && get_balance(root->right) <= 0)
        return left_rotate(root);
    // RL
    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    return root;
}

void avl_free(AVLNode* node) {
    if (!node) return;
    avl_free(node->left);
    avl_free(node->right);
    free(node);
}

// ===== main =====
int main(void) {
    const char* filename = "dataset_id_ascending.csv"; // 실제 파일명에 맞게 수정

    Student* original = (Student*)malloc(sizeof(Student) * MAX_STUDENTS);
    if (!original) {
        perror("malloc");
        return 1;
    }

    int n = load_students(filename, original, MAX_STUDENTS);
    if (n <= 0) {
        fprintf(stderr, "학생 데이터를 불러오지 못했습니다.\n");
        free(original);
        return 1;
    }

    printf("읽은 학생 수: %d명\n", n);

    // ID 최대값 찾기 (새로운 key 생성용)
    int i;
    int max_id = original[0].id;
    for (i = 1; i < n; ++i) {
        if (original[i].id > max_id)
            max_id = original[i].id;
    }

    // 공통 연산 시퀀스 생성
    Operation ops[TRIALS];
    int extra_ids[TRIALS];
    int extra_cnt = 0;

    srand(1); // 재현 가능하게 고정

    // 1~20번: 검색 (기존 ID 중에서)
    for (i = 0; i < 20; ++i) {
        ops[i].op = 2;
        ops[i].key = original[rand() % n].id;
    }
    // 21~30번: 삽입 (새로운 ID들, 모두 성공하도록 max_id보다 크게 생성)
    for (i = 20; i < 30; ++i) {
        ops[i].op = 0;
        ops[i].key = max_id + (i - 19); // max_id+1, +2, ...
        extra_ids[extra_cnt++] = ops[i].key;
    }
    // 31~40번: 삭제 (방금 삽입한 ID들 삭제)
    for (i = 30; i < 40; ++i) {
        ops[i].op = 1;
        ops[i].key = extra_ids[i - 30];
    }

    // 비정렬/정렬 배열용 버퍼
    int capacity = n + TRIALS + 10;
    Student* unsorted = (Student*)malloc(sizeof(Student) * capacity);
    Student* sorted = (Student*)malloc(sizeof(Student) * capacity);
    if (!unsorted || !sorted) {
        perror("malloc");
        free(original);
        if (unsorted) free(unsorted);
        if (sorted) free(sorted);
        return 1;
    }

    // ============================== A) 비정렬 배열 ==============================
    memcpy(unsorted, original, sizeof(Student) * n);
    int unsorted_n = n;

    printf("\n========================================\n");
    printf(" A) 정렬되지 않은 배열 (Unsorted Array, 순차 탐색)\n");
    printf("========================================\n");
    printf("시도 | key      | op(0=ins,1=del,2=search) | 비교횟수 | 누적비교 | 결과\n");

    long long cum_unsorted = 0;
    for (i = 0; i < TRIALS; ++i) {
        long long comp = 0;
        int success = 0;

        if (ops[i].op == 0) {
            success = insert_unsorted(unsorted, &unsorted_n, capacity, ops[i].key, &comp);
        }
        else if (ops[i].op == 1) {
            success = delete_unsorted(unsorted, &unsorted_n, ops[i].key, &comp);
        }
        else { // search
            int idx = seq_search_unsorted(unsorted, unsorted_n, ops[i].key, &comp);
            success = (idx != -1);
        }

        cum_unsorted += comp;
        printf("%3d | %8d | %24d | %8lld | %9lld | %s\n",
            i + 1, ops[i].key, ops[i].op,
            comp, cum_unsorted,
            success ? "OK" : "FAIL");
    }

    // ============================== B) 정렬 배열 ==============================
    memcpy(sorted, original, sizeof(Student) * n);
    insertion_sort_by_id(sorted, n);
    int sorted_n = n;

    printf("\n========================================\n");
    printf(" B) 정렬된 배열 (Sorted Array, 이진 탐색)\n");
    printf("========================================\n");
    printf("시도 | key      | op(0=ins,1=del,2=search) | 비교횟수 | 누적비교 | 결과\n");

    long long cum_sorted = 0;
    for (i = 0; i < TRIALS; ++i) {
        long long comp = 0;
        int success = 0;

        if (ops[i].op == 0) {
            success = insert_sorted(sorted, &sorted_n, capacity, ops[i].key, &comp);
        }
        else if (ops[i].op == 1) {
            success = delete_sorted(sorted, &sorted_n, ops[i].key, &comp);
        }
        else {
            int idx = binary_search_sorted(sorted, sorted_n, ops[i].key, &comp);
            success = (idx != -1);
        }

        cum_sorted += comp;
        printf("%3d | %8d | %24d | %8lld | %9lld | %s\n",
            i + 1, ops[i].key, ops[i].op,
            comp, cum_sorted,
            success ? "OK" : "FAIL");
    }

    // ============================== C) AVL 트리 ==============================
    AVLNode* root = NULL;
    long long dummy_comp;
    int dummy_success;

    // 초기 트리 구성 (변환 단계: 비교 횟수는 실험에 포함하지 않음)
    for (i = 0; i < n; ++i) {
        dummy_comp = 0;
        dummy_success = 0;
        root = avl_insert(root, original[i], &dummy_comp, &dummy_success);
    }

    printf("\n========================================\n");
    printf(" C) AVL 트리\n");
    printf("========================================\n");
    printf("시도 | key      | op(0=ins,1=del,2=search) | 비교횟수 | 누적비교 | 결과\n");

    long long cum_avl = 0;
    for (i = 0; i < TRIALS; ++i) {
        long long comp = 0;
        int success = 0;

        if (ops[i].op == 0) {
            Student s;
            memset(&s, 0, sizeof(s));
            s.id = ops[i].key;
            root = avl_insert(root, s, &comp, &success);
        }
        else if (ops[i].op == 1) {
            success = 0;
            root = avl_delete(root, ops[i].key, &comp, &success);
        }
        else {
            AVLNode* node = avl_search(root, ops[i].key, &comp);
            success = (node != NULL);
        }

        cum_avl += comp;
        printf("%3d | %8d | %24d | %8lld | %9lld | %s\n",
            i + 1, ops[i].key, ops[i].op,
            comp, cum_avl,
            success ? "OK" : "FAIL");
    }

    avl_free(root);
    free(original);
    free(unsorted);
    free(sorted);

    return 0;
}
