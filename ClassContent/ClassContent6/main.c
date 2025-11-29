#define _CRT_SECURE_NO_WARNINGS

/******************************************************
 * SECTION 1 : 공통 구조체 & 전역 변수
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAX_STUDENTS 33000
#define NAME_LEN     64
#define NUM_TRIALS   40

 /* ------------------------------------------
  * Student 구조체
  * ------------------------------------------
  * CSV 파일에서 읽어오는 학생 데이터 저장.
  * mul_of_score = 국영수 점수의 곱
  * 모든 탐색/정렬 기준이 되는 핵심 필드.
  */
typedef struct {
    int id;
    char name[NAME_LEN];
    char gender;
    int korean;
    int english;
    int math;
    int mul_of_score;
} Student;

/* 원본 / 정렬 배열 */
Student students[MAX_STUDENTS];
Student sorted_students[MAX_STUDENTS];

/* 정렬 비교 횟수 저장 */
long long g_sort_comp_count = 0;

/* CSV 로딩 시 사용되는 마지막 ID */
int last_id = 0;

/******************************************************
 * SECTION 2 : CSV 파일 로딩 & 파싱  (MSVC 호환)
 ******************************************************/

 /*
  * parse_student_line()
  * ------------------------------------------
  * CSV 1줄을 Student 구조체에 파싱
  * MSVC 환경 → strtok_s 사용
  * return 1 = 성공, 0 = 실패
  */
int parse_student_line(const char* line, Student* stu)
{
    char buf[256];
    strcpy(buf, line);

    char* ctx = NULL;
    char* token = strtok_s(buf, ",\n", &ctx);
    if (!token) return 0;
    stu->id = atoi(token);
    last_id = stu->id;

    token = strtok_s(NULL, ",\n", &ctx);
    if (!token) return 0;
    strcpy(stu->name, token);

    token = strtok_s(NULL, ",\n", &ctx);
    if (!token) return 0;
    stu->gender = token[0];

    token = strtok_s(NULL, ",\n", &ctx);
    if (!token) return 0;
    stu->korean = atoi(token);

    token = strtok_s(NULL, ",\n", &ctx);
    if (!token) return 0;
    stu->english = atoi(token);

    token = strtok_s(NULL, ",\n", &ctx);
    if (!token) return 0;
    stu->math = atoi(token);

    stu->mul_of_score = stu->korean * stu->english * stu->math;
    return 1;
}

/*
 * load_csv_file()
 * ------------------------------------------
 * students.csv 읽어서 students[]에 저장.
 */
int load_csv_file(const char* filename)
{
    FILE* fp;
    char line[256];
    int n = 0;

    if (fopen_s(&fp, filename, "r") != 0) {
        printf("파일 열기 실패: %s\n", filename);
        return -1;
    }

    fgets(line, sizeof(line), fp);  // 헤더 버림

    while (fgets(line, sizeof(line), fp)) {
        if (parse_student_line(line, &students[n]))
            n++;
    }

    fclose(fp);
    return n;
}


/******************************************************
 * SECTION 3 : 선형 탐색 / 이진 탐색 / 정렬(qsort)
 ******************************************************/

 /*
  * sequential_search()
  * ------------------------------------------
  * 정렬되지 않은 배열에서 선형 탐색.
  * 반환: index / -1
  * comp: 비교 횟수 기록
  */
int sequential_search(Student arr[], int n, int key, int* comp)
{
    *comp = 0;

    for (int i = 0; i < n; i++) {
        (*comp)++;
        if (arr[i].mul_of_score == key)
            return i;
    }
    return -1;
}

/*
 * binary_search()
 * ------------------------------------------
 * 정렬 배열에서 이진 탐색.
 * 반환: index / -1
 * comp: 비교 횟수 기록
 */
int binary_search(Student arr[], int n, int key, int* comp)
{
    int low = 0, high = n - 1;
    *comp = 0;

    while (low <= high) {
        int mid = (low + high) / 2;
        (*comp)++;

        if (arr[mid].mul_of_score == key)
            return mid;

        if (key < arr[mid].mul_of_score)
            high = mid - 1;
        else
            low = mid + 1;
    }
    return -1;
}

/*
 * compare_by_mul()
 * ------------------------------------------
 * qsort 비교 함수
 * mul_of_score 기준 → tie-break: id
 */
int compare_by_mul(const void* a, const void* b)
{
    const Student* s1 = a;
    const Student* s2 = b;

    g_sort_comp_count++;

    if (s1->mul_of_score < s2->mul_of_score) return -1;
    if (s1->mul_of_score > s2->mul_of_score) return 1;
    return (s1->id - s2->id);
}

/******************************************************
 * SECTION 4 : 배열 삽입/삭제 (Unsorted + Sorted)
 ******************************************************/

 /*
  * unsorted_insert()
  * ------------------------------------------
  * 정렬되지 않은 배열 끝에 삽입.
  * 먼저 순차탐색으로 중복 체크 → 비교 횟수 comp 이용.
  */
int unsorted_insert(Student arr[], int* n, int key, int* comp, int* last_id_ref)
{
    int idx = sequential_search(arr, *n, key, comp);
    if (idx != -1) return 0;

    if (*n >= MAX_STUDENTS) return 0;

    (*last_id_ref)++;

    Student s;
    s.id = *last_id_ref;
    s.gender = (rand() % 2) ? 'M' : 'F';
    s.korean = rand() % 101;
    s.english = rand() % 101;
    s.math = rand() % 101;
    s.mul_of_score = s.korean * s.english * s.math;
    strcpy(s.name, "RANDOM");

    arr[*n] = s;
    (*n)++;
    return 1;
}

/*
 * unsorted_delete()
 * ------------------------------------------
 * 찾은 요소를 마지막 요소로 덮어서 삭제.
 */
int unsorted_delete(Student arr[], int* n, int key, int* comp)
{
    int idx = sequential_search(arr, *n, key, comp);
    if (idx == -1) return 0;

    arr[idx] = arr[*n - 1];
    (*n)--;
    return 1;
}

/*
 * lower_bound()
 * ------------------------------------------
 * 정렬된 배열에서 key 들어갈 위치 반환.
 * 이진탐색 기반이므로 비교 횟수 comp 증가.
 */
int lower_bound(Student arr[], int n, int key, int* comp)
{
    int low = 0, high = n;
    *comp = 0;

    while (low < high) {
        int mid = (low + high) / 2;

        (*comp)++;

        if (arr[mid].mul_of_score < key)
            low = mid + 1;
        else
            high = mid;
    }
    return low;
}

/*
 * sorted_insert()
 * ------------------------------------------
 * 정렬 배열 유지하며 삽입.
 * 위치 찾기 + shift 이동 횟수 comp에 기록.
 */
int sorted_insert(Student arr[], int* n, int key, int* comp, int* last_id_ref)
{
    if (*n >= MAX_STUDENTS) return 0;

    int c = 0;
    int pos = lower_bound(arr, *n, key, &c);
    *comp = c;

    if (pos < *n) {
        (*comp)++;
        if (arr[pos].mul_of_score == key)
            return 0;
    }

    for (int i = *n; i > pos; i--) {
        arr[i] = arr[i - 1];
        (*comp)++;
    }

    Student s;
    s.id = ++(*last_id_ref);
    s.gender = 'M';
    s.korean = s.english = s.math = 0;
    s.mul_of_score = key;
    strcpy(s.name, "DUMMY");

    arr[pos] = s;
    (*n)++;
    return 1;
}

/*
 * sorted_delete()
 * ------------------------------------------
 * 정렬 배열에서 삭제 → shift 발생
 */
int sorted_delete(Student arr[], int* n, int key, int* comp)
{
    int idx = binary_search(arr, *n, key, comp);
    if (idx == -1) return 0;

    for (int i = idx; i < *n - 1; i++)
        arr[i] = arr[i + 1];

    (*n)--;
    return 1;
}

/******************************************************
 * SECTION 5 : 보간 탐색 (Interpolation Search)
 ******************************************************/

 /*
  * interpolation_search()
  * ------------------------------------------
  * 이진탐색보다 더 빠른 탐색이 가능 (균등 분포일 때)
  * 비교 횟수 comp 기록
  */
int interpolation_search(Student arr[], int n, int key, int* comp)
{
    int low = 0, high = n - 1;
    *comp = 0;

    while (low <= high &&
        key >= arr[low].mul_of_score &&
        key <= arr[high].mul_of_score) {

        (*comp)++;

        long long pos =
            low +
            (long long)(key - arr[low].mul_of_score) *
            (high - low) /
            (arr[high].mul_of_score - arr[low].mul_of_score);

        if (pos < 0 || pos >= n)
            return -1;

        if (arr[pos].mul_of_score == key)
            return pos;

        if (arr[pos].mul_of_score < key)
            low = pos + 1;
        else
            high = pos - 1;
    }
    return -1;
}

/******************************************************
 * SECTION 6 : AVL 트리 (삽입 / 삭제 / 탐색)
 ******************************************************/

 /*
  * AVLNode
  * ------------------------------------------
  * key        : mul_of_score 값을 저장
  * data       : 해당 Student 전체 복사본
  * height     : 서브트리 높이
  * left/right : 왼쪽/오른쪽 자식 포인터
  */
typedef struct AVLNode {
    int key;
    Student data;
    int height;
    struct AVLNode* left;
    struct AVLNode* right;
} AVLNode;

/* AVL 연산별 비교 횟수 카운터 */
long long avl_comp_search = 0;
long long avl_comp_insert = 0;
long long avl_comp_delete = 0;

/* 헬퍼 함수들 --------------------------------------------------*/

/* 노드 높이 반환 (NULL이면 0) */
int avl_height(AVLNode* n) {
    return n ? n->height : 0;
}

/* 두 수 중 큰 값 반환 */
int avl_max(int a, int b) {
    return (a > b) ? a : b;
}

/* 현재 노드의 balance factor (왼쪽 높이 - 오른쪽 높이) */
int avl_get_balance(AVLNode* n) {
    if (!n) return 0;
    return avl_height(n->left) - avl_height(n->right);
}

/*
 * 오른쪽 회전 (Right Rotation)
 *
 *        y                x
 *       / \              / \
 *      x   T3   -->     T1  y
 *     / \                  / \
 *    T1  T2               T2 T3
 */
AVLNode* avl_rotate_right(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + avl_max(avl_height(y->left), avl_height(y->right));
    x->height = 1 + avl_max(avl_height(x->left), avl_height(x->right));

    return x;
}

/*
 * 왼쪽 회전 (Left Rotation)
 *
 *    x                      y
 *   / \                    / \
 *  T1  y       -->        x  T3
 *     / \                / \
 *    T2 T3              T1 T2
 */
AVLNode* avl_rotate_left(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + avl_max(avl_height(x->left), avl_height(x->right));
    y->height = 1 + avl_max(avl_height(y->left), avl_height(y->right));

    return y;
}

/*
 * avl_new_node()
 * ------------------------------------------
 * Student 한 명을 담는 새로운 AVL 노드 동적할당
 */
AVLNode* avl_new_node(Student s) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        fprintf(stderr, "AVL 노드 할당 실패\n");
        exit(1);
    }
    node->key = s.mul_of_score;
    node->data = s;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

/*
 * avl_search()
 * ------------------------------------------
 * key(mul_of_score)로 AVL 트리에서 탐색.
 * 각 비교마다 avl_comp_search 증가.
 *
 * return: 찾은 노드 포인터 / NULL
 */
AVLNode* avl_search(AVLNode* root, int key) {
    if (!root) return NULL;

    avl_comp_search++;

    if (key == root->key)
        return root;
    else if (key < root->key)
        return avl_search(root->left, key);
    else
        return avl_search(root->right, key);
}

/*
 * avl_insert()
 * ------------------------------------------
 * AVL 트리에 Student 삽입.
 * key = s.mul_of_score
 * - 이진 탐색 트리 삽입 과정에서 avl_comp_insert 증가.
 * - 삽입 후 balance 맞추기 위해 회전 수행.
 */
AVLNode* avl_insert(AVLNode* root, Student s) {
    /* 1) 일반 BST 삽입 */
    if (!root)
        return avl_new_node(s);

    avl_comp_insert++;  // 비교 횟수 증가

    if (s.mul_of_score < root->key)
        root->left = avl_insert(root->left, s);
    else if (s.mul_of_score > root->key)
        root->right = avl_insert(root->right, s);
    else
        return root;   // 동일 key는 삽입하지 않음

    /* 2) 높이 갱신 */
    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));

    /* 3) balance factor 계산 */
    int balance = avl_get_balance(root);

    /* 4) 4가지 회전 경우 처리 */

    // LL Case
    if (balance > 1 && s.mul_of_score < root->left->key)
        return avl_rotate_right(root);

    // RR Case
    if (balance < -1 && s.mul_of_score > root->right->key)
        return avl_rotate_left(root);

    // LR Case
    if (balance > 1 && s.mul_of_score > root->left->key) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }

    // RL Case
    if (balance < -1 && s.mul_of_score < root->right->key) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }

    return root;
}

/*
 * avl_min_node()
 * ------------------------------------------
 * 가장 왼쪽(최소 key) 노드 찾기 (삭제에서 사용)
 */
AVLNode* avl_min_node(AVLNode* root) {
    AVLNode* cur = root;
    while (cur && cur->left)
        cur = cur->left;
    return cur;
}

/*
 * avl_delete()
 * ------------------------------------------
 * AVL 트리에서 key 삭제.
 * - avl_comp_delete 비교 횟수 증가
 * - 삭제 후 높이 갱신 및 회전으로 균형 유지
 */
AVLNode* avl_delete(AVLNode* root, int key) {
    if (!root) return root;

    avl_comp_delete++;  // 비교 횟수 증가

    /* 1) 일반 BST 삭제 */
    if (key < root->key) {
        root->left = avl_delete(root->left, key);
    }
    else if (key > root->key) {
        root->right = avl_delete(root->right, key);
    }
    else {
        /* 삭제 대상 노드 발견 */
        if (!root->left || !root->right) {
            AVLNode* temp = root->left ? root->left : root->right;

            if (!temp) {
                /* 자식이 없는 경우 */
                temp = root;
                root = NULL;
            }
            else {
                /* 자식 하나 */
                *root = *temp;  // 내용 복사
            }
            free(temp);
        }
        else {
            /* 자식 둘인 경우: 오른쪽 서브트리에서 최소 노드 찾아서 교체 */
            AVLNode* temp = avl_min_node(root->right);
            root->key = temp->key;
            root->data = temp->data;
            root->right = avl_delete(root->right, temp->key);
        }
    }

    /* 삭제 후 빈 트리일 수 있음 */
    if (!root) return root;

    /* 2) 높이 갱신 */
    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));

    /* 3) balance factor 확인 */
    int balance = avl_get_balance(root);

    /* 4) 4가지 회전 경우 처리 */

    // LL Case
    if (balance > 1 && avl_get_balance(root->left) >= 0)
        return avl_rotate_right(root);

    // LR Case
    if (balance > 1 && avl_get_balance(root->left) < 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }

    // RR Case
    if (balance < -1 && avl_get_balance(root->right) <= 0)
        return avl_rotate_left(root);

    // RL Case
    if (balance < -1 && avl_get_balance(root->right) > 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }

    return root;
}

/*
 * avl_free()
 * ------------------------------------------
 * 동적 할당된 AVL 노드 전체 해제 (후위 순회)
 */
void avl_free(AVLNode* root) {
    if (!root) return;
    avl_free(root->left);
    avl_free(root->right);
    free(root);
}

/******************************************************
 * SECTION 7 : main() - 전체 실험 흐름
 ******************************************************/

int main(void)
{
    /* 1. CSV 로딩 */
    int n = load_csv_file("students.csv");
    if (n <= 0) {
        printf("학생 데이터를 읽지 못했습니다.\n");
        return 1;
    }

    printf("읽은 학생 수: %d명\n", n);

    /* 2. 정렬용 배열 초기화 + qsort 정렬 */
    for (int i = 0; i < n; i++)
        sorted_students[i] = students[i];

    g_sort_comp_count = 0;
    qsort(sorted_students, n, sizeof(Student), compare_by_mul);
    printf("[정렬 완료] qsort 비교 횟수: %lld\n", g_sort_comp_count);

    /* 3. 난수 키 생성 (모든 실험에서 공통 사용) */
    srand((unsigned)time(NULL));
    int keys[NUM_TRIALS];
    for (int i = 0; i < NUM_TRIALS; i++) {
        keys[i] = rand() % 1000001;  // 0 ~ 1,000,000
    }

    /**************************************************
     * A) 정렬되지 않은 배열 (삽입/삭제/순차 탐색)
     **************************************************/
    printf("\n========================================\n");
    printf(" A) 정렬되지 않은 배열 (Unsorted Array)\n");
    printf("========================================\n");
    printf("시도 | key      | op(0=ins,1=del,2=seq) | 비교횟수 | 누적비교\n");

    int n_uns = n;                 // 현재 사용 중인 원소 수
    long long uns_total = 0;       // 누적 비교 횟수
    int last_id_uns = last_id;     // 새로운 ID 발급용

    for (int t = 0; t < NUM_TRIALS; t++) {
        int key = keys[t];
        int comp = 0;
        int op = key % 3;          // 0: 삽입, 1: 삭제, 2: 순차탐색
        int ok = 0;

        if (op == 0) {
            ok = unsorted_insert(students, &n_uns, key, &comp, &last_id_uns);
        }
        else if (op == 1) {
            ok = unsorted_delete(students, &n_uns, key, &comp);
        }
        else {
            sequential_search(students, n_uns, key, &comp);
            ok = 1;                // 탐색 자체는 "시도 성공"으로 간주
        }

        uns_total += comp;
        printf("%3d | %8d | %16d | %7d | %9lld (%s)\n",
            t + 1, key, op, comp, uns_total, ok ? "OK" : "FAIL");
    }

    /**************************************************
     * B) 정렬된 배열 (삽입/삭제/이진 탐색)
     **************************************************/
    printf("\n========================================\n");
    printf(" B) 정렬된 배열 (Sorted Array)\n");
    printf("========================================\n");
    printf("시도 | key      | op(0=ins,1=del,2=bin) | 비교횟수 | 누적비교\n");

    int n_sort = n;
    long long sort_total = 0;
    int last_id_sort = last_id;

    for (int t = 0; t < NUM_TRIALS; t++) {
        int key = keys[t];
        int comp = 0;
        int op = key % 3;          // 0: 삽입, 1: 삭제, 2: 이진탐색
        int ok = 0;

        if (op == 0) {
            ok = sorted_insert(sorted_students, &n_sort, key, &comp, &last_id_sort);
        }
        else if (op == 1) {
            ok = sorted_delete(sorted_students, &n_sort, key, &comp);
        }
        else {
            binary_search(sorted_students, n_sort, key, &comp);
            ok = 1;
        }

        sort_total += comp;
        printf("%3d | %8d | %16d | %7d | %9lld (%s)\n",
            t + 1, key, op, comp, sort_total, ok ? "OK" : "FAIL");
    }

    /**************************************************
     * C) AVL 트리 (초기 삽입 + 탐색 + 삭제)
     **************************************************/
    printf("\n========================================\n");
    printf(" C) AVL 트리 (AVL Tree)\n");
    printf("========================================\n");

    AVLNode* avl_root = NULL;

    /* 전체 학생을 AVL에 삽입 */
    avl_comp_insert = avl_comp_search = avl_comp_delete = 0;
    for (int i = 0; i < n; i++) {
        avl_root = avl_insert(avl_root, students[i]);
    }

    printf("AVL 초기 삽입 비교 횟수: %lld\n", avl_comp_insert);

    /* 임의의 key 하나로 탐색/삭제 실험 */
    int testKey = keys[0];

    avl_comp_search = 0;
    AVLNode* found = avl_search(avl_root, testKey);
    printf("AVL 탐색 (key=%d) 비교 횟수: %lld (found=%s)\n",
        testKey, avl_comp_search, found ? "YES" : "NO");

    avl_comp_delete = 0;
    avl_root = avl_delete(avl_root, testKey);
    printf("AVL 삭제 (key=%d) 비교 횟수: %lld\n", testKey, avl_comp_delete);

    /**************************************************
 * C-2) AVL 트리: 40회 삽입/삭제/탐색 실험
 **************************************************/
    printf("\n----------------------------------------\n");
    printf(" C-2) AVL 40회 실험 (insert/delete/search)\n");
    printf("----------------------------------------\n");
    printf("시도 | key      | op | 비교횟수 | 누적비교 | 결과\n");

    long long avl_total = 0;   // 누적 비교 횟수

    for (int t = 0; t < 40; t++) {

        int key = keys[t];
        int op = key % 3;   // 0: insert, 1: delete, 2: search
        int comp_before, comp_after;
        int success = 0;

        if (op == 0) {
            /* INSERT */
            comp_before = avl_comp_insert;
            avl_root = avl_insert(avl_root, students[key % n]);
            comp_after = avl_comp_insert;

            int diff = comp_after - comp_before;
            avl_total += diff;

            // 삽입 성공 여부 판단: key를 탐색해서 존재하면 성공
            AVLNode* chk = avl_search(avl_root, students[key % n].mul_of_score);
            success = (chk != NULL);

            printf("%3d | %8d |  0 | %8d | %10lld | %s\n",
                t + 1, key, diff, avl_total, success ? "OK" : "FAIL");
        }
        else if (op == 1) {
            /* DELETE */
            comp_before = avl_comp_delete;

            // 삭제 전 탐색 (존재 여부)
            AVLNode* before = avl_search(avl_root, key);

            avl_comp_delete = comp_before;
            avl_root = avl_delete(avl_root, key);
            comp_after = avl_comp_delete;

            int diff = comp_after - comp_before;
            avl_total += diff;

            // 삭제 성공 여부 판단
            AVLNode* after = avl_search(avl_root, key);
            success = (before != NULL && after == NULL);

            printf("%3d | %8d |  1 | %8d | %10lld | %s\n",
                t + 1, key, diff, avl_total, success ? "OK" : "FAIL");
        }
        else {
            /* SEARCH */
            comp_before = avl_comp_search;
            AVLNode* found = avl_search(avl_root, key);
            comp_after = avl_comp_search;

            int diff = comp_after - comp_before;
            avl_total += diff;

            success = (found != NULL);

            printf("%3d | %8d |  2 | %8d | %10lld | %s\n",
                t + 1, key, diff, avl_total, success ? "OK" : "FAIL");
        }
    }

    /**************************************************
     * D) 보간 탐색 vs 이진 탐색 비교 (40회, 형식 통일)
     **************************************************/
    printf("\n========================================\n");
    printf(" D) 보간 탐색 vs 이진 탐색 비교 (40회)\n");
    printf("========================================\n");
    printf("시도 | key      | op(0=bin,1=interp) | 비교횟수 | 누적비교 | 결과\n");

    long long interp_total = 0;

    for (int t = 0; t < NUM_TRIALS; t++) {

        int key = keys[t];
        int comp_bin = 0, comp_int = 0;

        /* ------------------------
           이진 탐색 (op = 0)
           ------------------------ */
        binary_search(sorted_students, n_sort, key, &comp_bin);
        interp_total += comp_bin;

        printf("%3d | %8d |    0 (bin)       | %8d | %10lld | %s\n",
            t + 1, key, comp_bin, interp_total,
            (comp_bin > 0 ? "OK" : "FAIL"));

        /* ------------------------e
           보간 탐색 (op = 1)
           ------------------------ */
        binary_search(sorted_students, n_sort, key, &comp_bin);  // OK 여부 확인용
        int exists = (comp_bin > 0); // 이진 탐색에서 탐색 성공한 경우만 OK 처리

        interpolation_search(sorted_students, n_sort, key, &comp_int);
        interp_total += comp_int;

        printf("%3d | %8d |    1 (interp)    | %8d | %10lld | %s\n",
            t + 1, key, comp_int, interp_total,
            exists ? "OK" : "FAIL");
    }
}