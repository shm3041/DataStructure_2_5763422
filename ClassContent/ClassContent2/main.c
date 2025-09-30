// array_bt_char0_insert_at_delete.c

#include <stdio.h>

#include <stdlib.h>

#include <string.h>



typedef struct {

    char* a;       // 1-based: a[1..max_i], a[i]==0이면 빈 노드

    size_t cap;     // 현재 할당 용량

    size_t max_i;   // 사용된 최대 인덱스(중간이 비어있을 수 있음)

    size_t count;   // 실제 유효 노드 수 (a[i] != 0)

} ArrBT;



/* --- 유틸 / 인덱스 --- */

static void die(const char* m) { perror(m); exit(EXIT_FAILURE); }

static inline size_t left(size_t i) { return 2 * i; }

static inline size_t right(size_t i) { return 2 * i + 1; }

static inline size_t parent(size_t i) { return i / 2; }

static inline int is_upper(char c) { return c >= 'A' && c <= 'Z'; }



/* 필요 용량 보장: 새로 늘어난 구간은 0으로 채움 */

static void ensure_cap(ArrBT* t, size_t need) {

    if (need < t->cap) return;                 // cap은 0..cap-1 인덱스 크기

    size_t ncap = t->cap ? t->cap : 2;

    while (ncap <= need) ncap <<= 1;

    char* na = (char*)realloc(t->a, ncap * sizeof(char));

    if (!na) die("realloc");

    if (ncap > t->cap) memset(na + t->cap, 0, ncap - t->cap);  // 확장 영역 0으로 초기화

    t->a = na; t->cap = ncap;

}



/* -------- 생성/해제 -------- */

ArrBT* bt_create(size_t initial_cap) {

    if (initial_cap < 2) initial_cap = 2; // a[0] 미사용

    ArrBT* t = (ArrBT*)malloc(sizeof(ArrBT));

    if (!t) die("malloc ArrBT");



    t->a = (char*)malloc(initial_cap * sizeof(char));

    if (!t->a) die("malloc a");

    memset(t->a, 0, initial_cap);  // 전부 빈 노드(0)로



    t->cap = initial_cap;

    t->max_i = 0;

    t->count = 0;

    return t;

}



void bt_free(ArrBT* t) {

    if (!t) return;

    free(t->a);

    t->a = NULL;

    free(t);

}



/* -------- 삽입 API -------- */

/* i가 비어있을 때만 ch 삽입. 성공: i, 실패: 0

   - ch는 'A'~'Z'만 허용

   - 안전 체크: i!=1이면 parent(i)가 존재해야 함

*/

size_t insert_node(ArrBT* t, size_t i, char ch) {

    if (!t || i == 0 || !is_upper(ch)) return 0;



    // 부모 존재성 체크(루트 제외)

    if (i != 1) {

        size_t p = parent(i);

        ensure_cap(t, p);

        if (p == 0 || t->a[p] == 0) return 0;  // 부모 없음 → 실패

    }



    ensure_cap(t, i);

    if (t->a[i] != 0) return 0;  // 이미 값이 있음 → 실패



    t->a[i] = ch;

    t->count++;

    if (i > t->max_i) t->max_i = i;

    return i;

}



/* -------- 삭제 API (단말만) -------- */

/* 단말(leaf)일 때만 삭제. 성공: 1, 실패: 0 */

int node_delete(ArrBT* t, size_t i) {

    if (!t || i == 0 || i > t->max_i) return 0;

    if (t->a[i] == 0) return 0;  // 빈 칸



    size_t li = left(i), ri = right(i);

    int has_left = (li <= t->max_i && t->a[li] != 0);

    int has_right = (ri <= t->max_i && t->a[ri] != 0);



    if (has_left || has_right) return 0;  // 단말이 아님 → 실패



    // 삭제

    t->a[i] = 0;

    t->count--;



    // 뒤쪽 연속 빈 칸 정리하여 max_i 축소

    if (i == t->max_i) {

        while (t->max_i > 0 && t->a[t->max_i] == 0) t->max_i--;

    }

    return 1;

}



/* -------- 조회/통계/순회 -------- */

size_t bt_count(const ArrBT* t) { return t->count; }



/* 높이(간선 기준): 공백 -1, 리프 0 */

int height_idx(const ArrBT* t, size_t i) {

    if (i == 0 || i > t->max_i) return -1;

    if (t->a[i] == 0) return -1;

    int hl = height_idx(t, left(i));

    int hr = height_idx(t, right(i));

    return 1 + (hl > hr ? hl : hr);

}



/* 순회: 빈 노드(0)는 건너뜀 */

void preorder_idx(const ArrBT* t, size_t i) {

    if (i == 0 || i > t->max_i || t->a[i] == 0) return;

    printf("%c (", t->a[i]);

    preorder_idx(t, left(i));

    preorder_idx(t, right(i));

    printf(")");

}

void inorder_idx(const ArrBT* t, size_t i) {

    if (i == 0 || i > t->max_i || t->a[i] == 0) return;

    inorder_idx(t, left(i));

    printf("%c ", t->a[i]);

    inorder_idx(t, right(i));

}

void postorder_idx(const ArrBT* t, size_t i) {

    if (i == 0 || i > t->max_i || t->a[i] == 0) return;

    postorder_idx(t, left(i));

    postorder_idx(t, right(i));

    printf("%c ", t->a[i]);

}



/* 레벨 순회(존재 노드만 출력) */

void levelorder_present(const ArrBT* t) {

    for (size_t i = 1; i <= t->max_i; i++) {

        if (t->a[i]) printf("%c ", t->a[i]);

    }

}



/* 디버그: 인덱스 1..max_i 를 그대로 표시(빈 칸은 0으로 표기) */

void dump_array(const ArrBT* t) {

    printf("[1..%zu]: ", t->max_i);

    for (size_t i = 1; i <= t->max_i; i++) {

        if (t->a[i] == 0) printf("0 ");

        else printf("%c ", t->a[i]);

    }

    puts("");

}



/* -------- 데모 -------- */

int main(void) {

    ArrBT* t = bt_create(8);



    // 트리 구성

    insert_node(t, 1, 'A');  // 루트

    insert_node(t, 2, 'B');  // A의 왼쪽

    insert_node(t, 3, 'C');  // A의 오른쪽

    insert_node(t, 4, 'D');  // B의 왼쪽  (leaf)

    insert_node(t, 5, 'E');  // B의 오른쪽(leaf)

    // 6은 비움

    insert_node(t, 7, 'G');  // C의 오른쪽(leaf)



    puts("초기 상태:");

    dump_array(t);



    puts("전위 순회:");

    preorder_idx(t, 1);



    puts("\n");

    // 단말 삭제 시도

    if (node_delete(t, 4)) puts("i=4(D) 삭제 성공"); else puts("i=4 삭제 실패");

    if (node_delete(t, 2)) puts("i=2(B) 삭제 성공"); else puts("i=2 삭제 실패(단말 아님)");



    puts("삭제 후:");

    dump_array(t);



    printf("노드 수(count): %zu\n", bt_count(t));

    printf("높이(간선 기준): %d\n", height_idx(t, 1));



    bt_free(t);

    return 0;

}