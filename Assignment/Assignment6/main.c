#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* ==== 공통 ==== */
enum { V = 100, SPARSE_E = 100, DENSE_E = 4000 };
typedef struct { int u, v; } Edge;

typedef struct {
    long long cmp_ins_del;
    long long cmp_conn;
    long long cmp_list;
} Cnts;

typedef struct {
    int conn_u, conn_v;  /* 연결 여부 확인 쿼리 (존재하는 간선에서 선택) */
    int mod_u, mod_v;   /* 삽입/삭제 쿼리 (현재 없는 간선에서 선택) */
    int list_u;          /* 인접 노드 출력 대상 정점 (평균 차수에 가장 근접) */
} Queries;

/* ==== 난수 유틸 ==== */
static int urand(int n) { return rand() % n; }

/* ==== 무작위 간선 생성: Fisher-Yates 셔플로 E개 샘플 ==== */
static void build_random_edges(int E, Edge* out, unsigned char present[V][V], int* deg) {
    const int TOT = V * (V - 1) / 2; /* 4950 */
    Edge* pool = (Edge*)malloc(sizeof(Edge) * TOT);
    int k = 0;
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            pool[k++] = (Edge){ i,j };
        }
    }
    /* partial shuffle: 앞에서 E개만 무작위 선택 */
    for (int i = 0; i < E; i++) {
        int r = i + urand(TOT - i);
        Edge tmp = pool[i]; pool[i] = pool[r]; pool[r] = tmp;
        out[i] = pool[i];
    }
    /* present/deg 채우기 */
    memset(present, 0, V * V * sizeof(unsigned char));
    memset(deg, 0, V * sizeof(int));
    for (int i = 0; i < E; i++) {
        int u = out[i].u, v = out[i].v;
        present[u][v] = present[v][u] = 1;
        deg[u]++; deg[v]++;
    }
    free(pool);
}

/* ==== 평균 차수에 가까운 정점, 존재/부재 간선 선택 ==== */
static int argmin_deg_to_mean(const int* deg, double mean) {
    int best = 0; double bestd = 1e9;
    for (int i = 0; i < V; i++) {
        double d = (deg[i] - mean); if (d < 0) d = -d;
        if (deg[i] > 0 && d < bestd) { bestd = d; best = i; }
    }
    /* 만약 전부 0인 경우(이론상 불가) 0 리턴 */
    return best;
}
static void pick_queries_from_random_graph(const Edge* es, int m,
    unsigned char present[V][V],
    const int* deg, Queries* q)
{
    double mean = (2.0 * m) / V;

    /* 인접 노드 출력: 평균 차수에 가장 가까운 정점 */
    q->list_u = argmin_deg_to_mean(deg, mean);

    /* 연결 확인: 존재 간선 하나 선택, 평균에 더 가까운 쪽을 기준으로 */
    int idx = urand(m);
    int u = es[idx].u, v = es[idx].v;
    double du = (deg[u] - mean); if (du < 0) du = -du;
    double dv = (deg[v] - mean); if (dv < 0) dv = -dv;
    if (du <= dv) { q->conn_u = u; q->conn_v = v; }
    else { q->conn_u = v; q->conn_v = u; }

    /* 삽입/삭제: 현재 없는 간선 중에서 list_u와 연결되지 않은 정점 v 무작위 선택 */
    int trials = 0;
    do {
        q->mod_u = q->list_u;
        q->mod_v = urand(V);
        if (q->mod_v == q->mod_u) continue;
        if (!present[q->mod_u][q->mod_v]) break;
        trials++;
    } while (trials < 100000);
    /* (이론상 실패 거의 없음) */
}

/* ========================= 인접 행렬 ========================= */
typedef struct {
    int n;
    uint8_t* mat; /* n*n */
} GM;
static GM* gm_create(int n) {
    GM* g = (GM*)malloc(sizeof(GM));
    g->n = n; g->mat = (uint8_t*)calloc((size_t)n * n, sizeof(uint8_t));
    return g;
}
static void gm_free(GM* g) { if (!g) return; free(g->mat); free(g); }

static void gm_add_edge(GM* g, int u, int v, Cnts* c) {
    c->cmp_ins_del += 1;
    if (!g->mat[(size_t)u * g->n + v]) {
        g->mat[(size_t)u * g->n + v] = 1;
        g->mat[(size_t)v * g->n + u] = 1;
    }
}
static void gm_del_edge(GM* g, int u, int v, Cnts* c) {
    c->cmp_ins_del += 1;
    if (g->mat[(size_t)u * g->n + v]) {
        g->mat[(size_t)u * g->n + v] = 0;
        g->mat[(size_t)v * g->n + u] = 0;
    }
}
static int gm_connected(GM* g, int u, int v, Cnts* c) {
    c->cmp_conn += 1;
    return g->mat[(size_t)u * g->n + v] != 0;
}
static void gm_list_neighbors(GM* g, int u, Cnts* c) {
    (void)u;
    for (int v = 0; v < g->n; ++v) { c->cmp_list += 1; }
}
static size_t gm_memory_bytes(GM* g) {
    return sizeof(GM) + (size_t)g->n * g->n * sizeof(uint8_t);
}

/* ========================= 인접 리스트(연결: 희소) ========================= */
typedef struct Node {
    int to; struct Node* next;
} Node;
typedef struct {
    int n;
    Node** head;
    int* deg;
} GLL;

static GLL* gll_create(int n) {
    GLL* g = (GLL*)malloc(sizeof(GLL));
    g->n = n; g->head = (Node**)calloc((size_t)n, sizeof(Node*));
    g->deg = (int*)calloc((size_t)n, sizeof(int));
    return g;
}
static void gll_free(GLL* g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) { Node* cur = g->head[i]; while (cur) { Node* nx = cur->next; free(cur); cur = nx; } }
    free(g->head); free(g->deg); free(g);
}
static void gll_add_one(GLL* g, int u, int v, Cnts* c) {
    Node** pp = &g->head[u]; Node* cur = *pp;
    while (cur && cur->to < v) { c->cmp_ins_del += 1; pp = &cur->next; cur = cur->next; }
    if (cur && cur->to == v) { c->cmp_ins_del += 1; return; }
    Node* nd = (Node*)malloc(sizeof(Node)); nd->to = v; nd->next = cur; *pp = nd; g->deg[u]++;
}
static void gll_add_edge(GLL* g, int u, int v, Cnts* c) { gll_add_one(g, u, v, c); gll_add_one(g, v, u, c); }
static void gll_del_one(GLL* g, int u, int v, Cnts* c) {
    Node** pp = &g->head[u]; Node* cur = *pp;
    while (cur && cur->to < v) { c->cmp_ins_del += 1; pp = &cur->next; cur = cur->next; }
    if (cur && cur->to == v) { c->cmp_ins_del += 1; *pp = cur->next; free(cur); g->deg[u]--; }
    else { if (cur) c->cmp_ins_del += 1; }
}
static void gll_del_edge(GLL* g, int u, int v, Cnts* c) { gll_del_one(g, u, v, c); gll_del_one(g, v, u, c); }
static int gll_connected(GLL* g, int u, int v, Cnts* c) {
    Node* cur = g->head[u];
    while (cur && cur->to < v) { c->cmp_conn += 1; cur = cur->next; }
    if (cur) { c->cmp_conn += 1; return cur->to == v; }
    return 0;
}
static void gll_list_neighbors(GLL* g, int u, Cnts* c) {
    Node* cur = g->head[u]; while (cur) { c->cmp_list += 1; cur = cur->next; }
}
static size_t gll_memory_bytes(GLL* g) {
    long long nodes = 0; for (int i = 0; i < g->n; i++) nodes += g->deg[i];
    return sizeof(GLL) + (size_t)g->n * sizeof(Node*) + (size_t)g->n * sizeof(int)
        + (size_t)nodes * sizeof(Node);
}

/* ========================= 인접 리스트(배열: 밀집) ========================= */
typedef struct {
    int n;
    int** adj; int* deg; int* cap;
} GLA;

static int cmp_int(const void* a, const void* b) { int x = *(const int*)a, y = *(const int*)b; return (x > y) - (x < y); }

static GLA* gla_build_from_edges(int n, const Edge* es, int m) {
    GLA* g = (GLA*)malloc(sizeof(GLA));
    g->n = n; g->deg = (int*)calloc((size_t)n, sizeof(int));
    g->cap = (int*)calloc((size_t)n, sizeof(int));
    g->adj = (int**)calloc((size_t)n, sizeof(int*));
    for (int i = 0; i < m; i++) { g->deg[es[i].u]++; g->deg[es[i].v]++; }
    for (int i = 0; i < n; i++) { g->cap[i] = g->deg[i]; g->adj[i] = (int*)malloc((g->cap[i] ? g->cap[i] : 1) * sizeof(int)); g->deg[i] = 0; }
    for (int i = 0; i < m; i++) { int u = es[i].u, v = es[i].v; g->adj[u][g->deg[u]++] = v; g->adj[v][g->deg[v]++] = u; }
    for (int i = 0; i < n; i++) { qsort(g->adj[i], (size_t)g->deg[i], sizeof(int), cmp_int); }
    return g;
}
static void gla_free(GLA* g) { if (!g) return; for (int i = 0; i < g->n; i++) free(g->adj[i]); free(g->adj); free(g->deg); free(g->cap); free(g); }

static int gla_bsearch_and_count(const int* arr, int len, int key, long long* cmp) {
    int lo = 0, hi = len - 1;
    while (lo <= hi) { int mid = lo + ((hi - lo) >> 1); (*cmp)++; if (arr[mid] == key) return mid; if (arr[mid] < key) lo = mid + 1; else hi = mid - 1; }
    return -(lo + 1);
}
static void gla_insert_one(GLA* g, int u, int v, Cnts* c) {
    int pos = gla_bsearch_and_count(g->adj[u], g->deg[u], v, &c->cmp_ins_del);
    if (pos >= 0) return;
    pos = -pos - 1;
    if (g->deg[u] == g->cap[u]) {
        int newcap = g->cap[u] ? g->cap[u] * 2 : 1;
        int* na = (int*)malloc((size_t)newcap * sizeof(int));
        if (pos > 0) memcpy(na, g->adj[u], (size_t)pos * sizeof(int));
        na[pos] = v;
        if (g->deg[u] - pos > 0) memcpy(na + pos + 1, g->adj[u] + pos, (size_t)(g->deg[u] - pos) * sizeof(int));
        free(g->adj[u]); g->adj[u] = na; g->cap[u] = newcap; g->deg[u] += 1;
    }
    else {
        memmove(g->adj[u] + pos + 1, g->adj[u] + pos, (size_t)(g->deg[u] - pos) * sizeof(int));
        g->adj[u][pos] = v; g->deg[u] += 1;
    }
}
static void gla_add_edge(GLA* g, int u, int v, Cnts* c) { gla_insert_one(g, u, v, c); gla_insert_one(g, v, u, c); }
static void gla_del_one(GLA* g, int u, int v, Cnts* c) {
    int pos = gla_bsearch_and_count(g->adj[u], g->deg[u], v, &c->cmp_ins_del);
    if (pos < 0) return;
    if (pos + 1 < g->deg[u]) memmove(g->adj[u] + pos, g->adj[u] + pos + 1, (size_t)(g->deg[u] - pos - 1) * sizeof(int));
    g->deg[u] -= 1;
}
static void gla_del_edge(GLA* g, int u, int v, Cnts* c) { gla_del_one(g, u, v, c); gla_del_one(g, v, u, c); }
static int gla_connected(GLA* g, int u, int v, Cnts* c) {
    int pos = gla_bsearch_and_count(g->adj[u], g->deg[u], v, &c->cmp_conn);
    return pos >= 0;
}
static void gla_list_neighbors(GLA* g, int u, Cnts* c) { c->cmp_list += g->deg[u]; }
static size_t gla_memory_bytes(GLA* g) {
    size_t sum = sizeof(GLA) + (size_t)g->n * sizeof(int*) + (size_t)g->n * sizeof(int) + (size_t)g->n * sizeof(int);
    for (int i = 0; i < g->n; i++) sum += (size_t)g->cap[i] * sizeof(int);
    return sum;
}

/* ==== 케이스 실행 ==== */
static void run_sparse_cases(void) {
    Edge es[SPARSE_E];
    unsigned char present[V][V];
    int deg[V];
    build_random_edges(SPARSE_E, es, present, deg);

    /* 쿼리 선택 */
    Queries q; pick_queries_from_random_graph(es, SPARSE_E, present, deg, &q);

    /* 케이스 1: 희소-인접행렬 */
    {
        GM* g = gm_create(V);
        for (int i = 0; i < SPARSE_E; i++) {
            int u = es[i].u, v = es[i].v;
            g->mat[(size_t)u * V + v] = 1; g->mat[(size_t)v * V + u] = 1;
        }
        Cnts c = { 0 }; size_t mem = gm_memory_bytes(g);
        (void)gm_connected(g, q.conn_u, q.conn_v, &c);
        gm_list_neighbors(g, q.list_u, &c);
        gm_add_edge(g, q.mod_u, q.mod_v, &c);
        gm_del_edge(g, q.mod_u, q.mod_v, &c);

        printf("케이스 1:\n");
        printf("메모리 %zu Bytes\n", mem);
        printf("간선 삽입/삭제 비교 %lld번\n", c.cmp_ins_del);
        printf("두 정점의 연결 확인 비교 %lld번\n", c.cmp_conn);
        printf("한 노드의 인접 노드 출력 비교 %lld번\n", c.cmp_list);
        gm_free(g);
    }

    /* 케이스 2: 희소-인접리스트(연결) */
    {
        GLL* g = gll_create(V);
        Cnts tmp = { 0 };
        for (int i = 0; i < SPARSE_E; i++) gll_add_edge(g, es[i].u, es[i].v, &tmp); /* 빌드 계수 제외 */
        size_t mem = gll_memory_bytes(g);

        Cnts c = { 0 };
        (void)gll_connected(g, q.conn_u, q.conn_v, &c);
        gll_list_neighbors(g, q.list_u, &c);
        gll_add_edge(g, q.mod_u, q.mod_v, &c);
        gll_del_edge(g, q.mod_u, q.mod_v, &c);

        printf("케이스 2:\n");
        printf("메모리 %zu Bytes\n", mem);
        printf("간선 삽입/삭제 비교 %lld번\n", c.cmp_ins_del);
        printf("두 정점의 연결 확인 비교 %lld번\n", c.cmp_conn);
        printf("한 노드의 인접 노드 출력 비교 %lld번\n", c.cmp_list);
        gll_free(g);
    }
}

static void run_dense_cases(void) {
    Edge es[DENSE_E];
    unsigned char present[V][V];
    int deg[V];
    build_random_edges(DENSE_E, es, present, deg);

    /* 쿼리 선택 */
    Queries q; pick_queries_from_random_graph(es, DENSE_E, present, deg, &q);

    /* 케이스 3: 밀집-인접행렬 */
    {
        GM* g = gm_create(V);
        for (int i = 0; i < DENSE_E; i++) {
            int u = es[i].u, v = es[i].v;
            g->mat[(size_t)u * V + v] = 1; g->mat[(size_t)v * V + u] = 1;
        }
        Cnts c = { 0 }; size_t mem = gm_memory_bytes(g);
        (void)gm_connected(g, q.conn_u, q.conn_v, &c);
        gm_list_neighbors(g, q.list_u, &c);
        gm_add_edge(g, q.mod_u, q.mod_v, &c);
        gm_del_edge(g, q.mod_u, q.mod_v, &c);

        printf("케이스 3:\n");
        printf("메모리 %zu Bytes\n", mem);
        printf("간선 삽입/삭제 비교 %lld번\n", c.cmp_ins_del);
        printf("두 정점의 연결 확인 비교 %lld번\n", c.cmp_conn);
        printf("한 노드의 인접 노드 출력 비교 %lld번\n", c.cmp_list);
        gm_free(g);
    }

    /* 케이스 4: 밀집-인접리스트(배열) */
    {
        GLA* g = gla_build_from_edges(V, es, DENSE_E);
        size_t mem = gla_memory_bytes(g);

        Cnts c = { 0 };
        (void)gla_connected(g, q.conn_u, q.conn_v, &c);
        gla_list_neighbors(g, q.list_u, &c);
        gla_add_edge(g, q.mod_u, q.mod_v, &c);
        gla_del_edge(g, q.mod_u, q.mod_v, &c);

        printf("케이스 4:\n");
        printf("메모리 %zu Bytes\n", mem);
        printf("간선 삽입/삭제 비교 %lld번\n", c.cmp_ins_del);
        printf("두 정점의 연결 확인 비교 %lld번\n", c.cmp_conn);
        printf("한 노드의 인접 노드 출력 비교 %lld번\n", c.cmp_list);
        gla_free(g);
    }
}

int main(void) {
    srand((unsigned)time(NULL));
    run_sparse_cases();
    run_dense_cases();
    return 0;
}
 