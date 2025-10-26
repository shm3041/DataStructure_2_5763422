#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* -------- ���� ���� -------- */
enum { V = 100, SPARSE_E = 100, DENSE_E = 4000 };

typedef struct { int u, v; } Edge;

typedef struct {
    long long cmp_ins_del;
    long long cmp_conn;
    long long cmp_list;
} Cnts;

/* -------- �׽�Ʈ�� ����� ���� ���� ���� (������, �ߺ� ����) -------- */
/* ���: (i, i+50) 50�� + (i, (i+1)%50) 50�� = 100�� */
static int build_sparse_edges(Edge* out) {
    int m = 0;
    for (int i = 0; i < 50; ++i) { out[m++] = (Edge){ i, i + 50 }; }
    for (int i = 0; i < 50; ++i) { out[m++] = (Edge){ i, (i + 1) % 50 }; }
    return m; /* 100 */
}

/* ����: (i<j) ���������� ä��� (0,99)�� �ǳʶٰ� �� 4000�� ä�� */
static int build_dense_edges(Edge* out) {
    int m = 0;
    for (int i = 0; i < V; ++i) {
        for (int j = i + 1; j < V; ++j) {
            if (i == 0 && j == 99) continue; /* ����/���� ��������� ����� */
            out[m++] = (Edge){ i, j };
            if (m == DENSE_E) return m;
        }
    }
    return m;
}

/* -------- ���̽� ����: ����� ����/���� -------- */
/* ���� ���� Ȯ���� (0,1)�� ���� (��� �������� �׻� ����).
   ����/������ (0,99)�� ���� (���� �������� ����ξ���, ��� �������� ���Ե��� �ʵ��� ������).
   ���� ��� ���Ŵ� ���� 0���� ����. */
enum { Q_U1 = 0, Q_V1 = 1, Q_U2 = 0, Q_V2 = 99, Q_LIST_U = 0 };

/* ========================= ���� ��� �׷��� ========================= */
typedef struct {
    int n;
    uint8_t* mat; /* n*n */
} GM;

static GM* gm_create(int n) {
    GM* g = (GM*)malloc(sizeof(GM));
    g->n = n;
    g->mat = (uint8_t*)calloc((size_t)n * n, sizeof(uint8_t));
    return g;
}
static void gm_free(GM* g) { if (!g) return; free(g->mat); free(g); }

static void gm_add_edge(GM* g, int u, int v, Cnts* c) {
    /* ���� �˻� 1ȸ�� ��� */
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
    c->cmp_conn += 1; /* �� ĭ Ȯ�� */
    return g->mat[(size_t)u * g->n + v] != 0;
}
static void gm_list_neighbors(GM* g, int u, Cnts* c) {
    /* u���� ������ ��ĵ: �� nȸ */
    for (int v = 0; v < g->n; ++v) {
        c->cmp_list += 1;
        (void)g->mat[(size_t)u * g->n + v];
    }
}
static size_t gm_memory_bytes(GM* g) {
    return sizeof(GM) + (size_t)g->n * g->n * sizeof(uint8_t);
}

/* ========================= ���� ����Ʈ(���� ����Ʈ: ��ҿ�) ========================= */
typedef struct Node {
    int to;
    struct Node* next;
} Node;

typedef struct {
    int n;
    Node** head; /* n�� ������ */
    int* deg;   /* �� ���� ���� */
} GLL; /* Linked List */

static GLL* gll_create(int n) {
    GLL* g = (GLL*)malloc(sizeof(GLL));
    g->n = n;
    g->head = (Node**)calloc((size_t)n, sizeof(Node*));
    g->deg = (int*)calloc((size_t)n, sizeof(int));
    return g;
}
static void gll_free(GLL* g) {
    if (!g) return;
    for (int i = 0; i < g->n; ++i) {
        Node* cur = g->head[i];
        while (cur) { Node* nx = cur->next; free(cur); cur = nx; }
    }
    free(g->head); free(g->deg); free(g);
}

static void gll_add_one(GLL* g, int u, int v, Cnts* c) {
    /* ���� ����: < �� �ݺ� + == �� �� �˻�(������ ��) */
    Node** pp = &g->head[u];
    Node* cur = *pp;
    while (cur && cur->to < v) {
        c->cmp_ins_del += 1;
        pp = &cur->next; cur = cur->next;
    }
    if (cur && cur->to == v) { c->cmp_ins_del += 1; return; } /* �ߺ� */
    Node* nd = (Node*)malloc(sizeof(Node));
    nd->to = v; nd->next = cur; *pp = nd; g->deg[u]++;
}
static void gll_add_edge(GLL* g, int u, int v, Cnts* c) {
    gll_add_one(g, u, v, c);
    gll_add_one(g, v, u, c);
}
static void gll_del_one(GLL* g, int u, int v, Cnts* c) {
    Node** pp = &g->head[u];
    Node* cur = *pp;
    while (cur && cur->to < v) {
        c->cmp_ins_del += 1;
        pp = &cur->next; cur = cur->next;
    }
    if (cur && cur->to == v) {
        c->cmp_ins_del += 1;
        *pp = cur->next; free(cur); g->deg[u]--;
    }
    else {
        if (cur) c->cmp_ins_del += 1; /* == ���� �� ī��Ʈ */
    }
}
static void gll_del_edge(GLL* g, int u, int v, Cnts* c) {
    gll_del_one(g, u, v, c);
    gll_del_one(g, v, u, c);
}
static int gll_connected(GLL* g, int u, int v, Cnts* c) {
    Node* cur = g->head[u];
    while (cur && cur->to < v) {
        c->cmp_conn += 1;
        cur = cur->next;
    }
    if (cur) { c->cmp_conn += 1; return cur->to == v; }
    return 0;
}
static void gll_list_neighbors(GLL* g, int u, Cnts* c) {
    Node* cur = g->head[u];
    while (cur) { c->cmp_list += 1; cur = cur->next; }
}
static size_t gll_memory_bytes(GLL* g) {
    /* ��� �� = 2E (������) */
    long long nodes = 0;
    for (int i = 0; i < g->n; ++i) nodes += g->deg[i];
    /* g ����ü + head �迭 + deg �迭 + ��� Node */
    return sizeof(GLL)
        + (size_t)g->n * sizeof(Node*)
        + (size_t)g->n * sizeof(int)
        + (size_t)nodes * sizeof(Node);
}

/* ========================= ���� ����Ʈ(�迭: ������) ========================= */
typedef struct {
    int n;
    int** adj;  /* �� ������ �̿� �迭(����) */
    int* deg;  /* ���� ���� */
    int* cap;  /* ���� �뷮 */
} GLA; /* List Array */

static int cmp_int(const void* a, const void* b) { int x = *(const int*)a, y = *(const int*)b; return (x > y) - (x < y); }

static GLA* gla_build_from_edges(int n, const Edge* es, int m) {
    GLA* g = (GLA*)malloc(sizeof(GLA));
    g->n = n;
    g->deg = (int*)calloc((size_t)n, sizeof(int));
    g->cap = (int*)calloc((size_t)n, sizeof(int));
    g->adj = (int**)calloc((size_t)n, sizeof(int*));
    /* 1) �� ���� ���� ���� */
    for (int i = 0; i < m; ++i) { g->deg[es[i].u]++; g->deg[es[i].v]++; }
    /* 2) ��Ȯ �뷮���� �Ҵ� */
    for (int i = 0; i < n; ++i) {
        g->cap[i] = g->deg[i];
        g->adj[i] = (int*)malloc((g->cap[i] ? g->cap[i] : 1) * sizeof(int));
        g->deg[i] = 0; /* ä��⸦ ���� 0���� �ʱ�ȭ */
    }
    /* 3) ä��� */
    for (int i = 0; i < m; ++i) {
        int u = es[i].u, v = es[i].v;
        g->adj[u][g->deg[u]++] = v;
        g->adj[v][g->deg[v]++] = u;
    }
    /* 4) ���� */
    for (int i = 0; i < n; ++i) {
        qsort(g->adj[i], (size_t)g->deg[i], sizeof(int), cmp_int);
    }
    return g;
}

static void gla_free(GLA* g) {
    if (!g) return;
    for (int i = 0; i < g->n; ++i) free(g->adj[i]);
    free(g->adj); free(g->deg); free(g->cap); free(g);
}

static int gla_bsearch_and_count(const int* arr, int len, int key, long long* cmp_counter) {
    int lo = 0, hi = len - 1;
    while (lo <= hi) {
        int mid = lo + ((hi - lo) >> 1);
        (*cmp_counter)++; /* key�� arr[mid] �� 1ȸ */
        if (arr[mid] == key) return mid;
        if (arr[mid] < key) lo = mid + 1;
        else hi = mid - 1;
    }
    return -(lo + 1);
}

static void gla_insert_one(GLA* g, int u, int v, Cnts* c) {
    int pos = gla_bsearch_and_count(g->adj[u], g->deg[u], v, &c->cmp_ins_del);
    if (pos >= 0) return; /* �̹� ���� */
    pos = -pos - 1;
    if (g->deg[u] == g->cap[u]) {
        int newcap = g->cap[u] ? g->cap[u] * 2 : 1;
        int* na = (int*)malloc((size_t)newcap * sizeof(int));
        /* ���� ���� */
        if (pos > 0) memcpy(na, g->adj[u], (size_t)pos * sizeof(int));
        /* ���� */
        na[pos] = v;
        /* ���� ���� */
        if (g->deg[u] - pos > 0)
            memcpy(na + pos + 1, g->adj[u] + pos, (size_t)(g->deg[u] - pos) * sizeof(int));
        free(g->adj[u]);
        g->adj[u] = na; g->cap[u] = newcap; g->deg[u] += 1;
    }
    else {
        /* �ڿ��� �� ĭ �б� */
        memmove(g->adj[u] + pos + 1, g->adj[u] + pos, (size_t)(g->deg[u] - pos) * sizeof(int));
        g->adj[u][pos] = v; g->deg[u] += 1;
    }
}
static void gla_add_edge(GLA* g, int u, int v, Cnts* c) {
    gla_insert_one(g, u, v, c);
    gla_insert_one(g, v, u, c);
}
static void gla_del_one(GLA* g, int u, int v, Cnts* c) {
    int pos = gla_bsearch_and_count(g->adj[u], g->deg[u], v, &c->cmp_ins_del);
    if (pos < 0) return;
    /* pos ���ĸ� �� ĭ ��� */
    if (pos + 1 < g->deg[u])
        memmove(g->adj[u] + pos, g->adj[u] + pos + 1, (size_t)(g->deg[u] - pos - 1) * sizeof(int));
    g->deg[u] -= 1;
}
static void gla_del_edge(GLA* g, int u, int v, Cnts* c) {
    gla_del_one(g, u, v, c);
    gla_del_one(g, v, u, c);
}
static int gla_connected(GLA* g, int u, int v, Cnts* c) {
    int pos = gla_bsearch_and_count(g->adj[u], g->deg[u], v, &c->cmp_conn);
    return pos >= 0;
}
static void gla_list_neighbors(GLA* g, int u, Cnts* c) {
    /* ���� ��ü �񱳴� deg(u)ȸ�� ���� */
    c->cmp_list += g->deg[u];
}
static size_t gla_memory_bytes(GLA* g) {
    size_t sum = sizeof(GLA)
        + (size_t)g->n * sizeof(int*) /* adj */
        + (size_t)g->n * sizeof(int)  /* deg */
        + (size_t)g->n * sizeof(int); /* cap */
    for (int i = 0; i < g->n; ++i) sum += (size_t)g->cap[i] * sizeof(int);
    return sum;
}

/* ========================= ���̽� ���� ========================= */
static void case_sparse_matrix(void) {
    Edge es[SPARSE_E]; int m = build_sparse_edges(es);
    GM* g = gm_create(V);
    /* ���� �ݿ� */
    for (int i = 0; i < m; ++i) {
        g->mat[(size_t)es[i].u * V + es[i].v] = 1;
        g->mat[(size_t)es[i].v * V + es[i].u] = 1;
    }
    Cnts c = { 0 };
    size_t mem = gm_memory_bytes(g);

    /* ���� Ȯ�� (0,1) */
    (void)gm_connected(g, Q_U1, Q_V1, &c);
    /* ���� ��� ���� (0) */
    gm_list_neighbors(g, Q_LIST_U, &c);
    /* ����/���� (0,99) */
    gm_add_edge(g, Q_U2, Q_V2, &c);
    gm_del_edge(g, Q_U2, Q_V2, &c);

    printf("���̽� 1:\n");
    printf("�޸� %zu Bytes\n", mem);
    printf("���� ����/���� �� %lld��\n", c.cmp_ins_del);
    printf("�� ������ ���� Ȯ�� �� %lld��\n", c.cmp_conn);
    printf("�� ����� ���� ��� ��� �� %lld��\n", c.cmp_list);

    gm_free(g);
}

static void case_sparse_list(void) {
    Edge es[SPARSE_E]; int m = build_sparse_edges(es);
    GLL* g = gll_create(V);
    Cnts c_build = { 0 };
    for (int i = 0; i < m; ++i) gll_add_edge(g, es[i].u, es[i].v, &c_build); /* ���� �񱳴� ��迡 �������� ���� */
    size_t mem = gll_memory_bytes(g);

    Cnts c = { 0 };
    (void)gll_connected(g, Q_U1, Q_V1, &c);
    gll_list_neighbors(g, Q_LIST_U, &c);
    gll_add_edge(g, Q_U2, Q_V2, &c);
    gll_del_edge(g, Q_U2, Q_V2, &c);

    printf("���̽� 2:\n");
    printf("�޸� %zu Bytes\n", mem);
    printf("���� ����/���� �� %lld��\n", c.cmp_ins_del);
    printf("�� ������ ���� Ȯ�� �� %lld��\n", c.cmp_conn);
    printf("�� ����� ���� ��� ��� �� %lld��\n", c.cmp_list);

    gll_free(g);
}

static void case_dense_matrix(void) {
    Edge es[DENSE_E]; int m = build_dense_edges(es);
    GM* g = gm_create(V);
    for (int i = 0; i < m; ++i) {
        g->mat[(size_t)es[i].u * V + es[i].v] = 1;
        g->mat[(size_t)es[i].v * V + es[i].u] = 1;
    }
    Cnts c = { 0 };
    size_t mem = gm_memory_bytes(g);

    (void)gm_connected(g, Q_U1, Q_V1, &c);
    gm_list_neighbors(g, Q_LIST_U, &c);
    gm_add_edge(g, Q_U2, Q_V2, &c);
    gm_del_edge(g, Q_U2, Q_V2, &c);

    printf("���̽� 3:\n");
    printf("�޸� %zu Bytes\n", mem);
    printf("���� ����/���� �� %lld��\n", c.cmp_ins_del);
    printf("�� ������ ���� Ȯ�� �� %lld��\n", c.cmp_conn);
    printf("�� ����� ���� ��� ��� �� %lld��\n", c.cmp_list);

    gm_free(g);
}

static void case_dense_list(void) {
    Edge es[DENSE_E]; int m = build_dense_edges(es);
    GLA* g = gla_build_from_edges(V, es, m);
    size_t mem = gla_memory_bytes(g);

    Cnts c = { 0 };
    (void)gla_connected(g, Q_U1, Q_V1, &c);
    gla_list_neighbors(g, Q_LIST_U, &c);
    gla_add_edge(g, Q_U2, Q_V2, &c);
    gla_del_edge(g, Q_U2, Q_V2, &c);

    printf("���̽� 4:\n");
    printf("�޸� %zu Bytes\n", mem);
    printf("���� ����/���� �� %lld��\n", c.cmp_ins_del);
    printf("�� ������ ���� Ȯ�� �� %lld��\n", c.cmp_conn);
    printf("�� ����� ���� ��� ��� �� %lld��\n", c.cmp_list);

    gla_free(g);
}

int main(void) {
    /* ��Ȯ�� 4�� ���̽��� ��� */
    case_sparse_matrix(); // ���-���
    case_sparse_list();   // ���-����Ʈ
    case_dense_matrix();  // ����-���
    case_dense_list();    // ����-����Ʈ
    return 0;
}
