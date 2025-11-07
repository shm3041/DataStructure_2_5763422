#define MAX_VERTICES 50

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int n;                         // 현재 정점 개수
    int adjMatrix[MAX_VERTICES][MAX_VERTICES]; // 인접 행렬
    int vertices[MAX_VERTICES];    // 정점 목록 (정수)
} Graph;

// ------------------------------
// 그래프 초기화
// ------------------------------
void createGraph(Graph* g) {
    g->n = 0;
    for (int i = 0; i < MAX_VERTICES; i++) {
        g->vertices[i] = -1;
        for (int j = 0; j < MAX_VERTICES; j++) {
            g->adjMatrix[i][j] = 0;
        }
    }
}

// ------------------------------
// 그래프가 비었는가 (간선이 없는가)
// ------------------------------
int isEmptyGraph(Graph* g) {
    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->n; j++) {
            if (g->adjMatrix[i][j] != 0)
                return 0; // 간선 존재
        }
    }
    return 1; // 간선 없음
}

// ------------------------------
// 정점 추가
// ------------------------------
void insertVertex(Graph* g, int v) {
    if (g->n >= MAX_VERTICES) {
        printf("그래프가 가득 찼습니다.\n");
        return;
    }
    g->vertices[g->n++] = v;
    printf("정점 %d 추가 완료\n", v);
}

// ------------------------------
// 정점 삭제
// ------------------------------
void deleteVertex(Graph* g, int v) {
    int idx = -1;
    for (int i = 0; i < g->n; i++) {
        if (g->vertices[i] == v) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("정점 %d를 찾을 수 없습니다.\n", v);
        return;
    }

    // 행과 열 삭제 (인접행렬 축소)
    for (int i = idx; i < g->n - 1; i++) {
        g->vertices[i] = g->vertices[i + 1];
        for (int j = 0; j < g->n; j++) {
            g->adjMatrix[i][j] = g->adjMatrix[i + 1][j];
        }
    }
    for (int j = idx; j < g->n - 1; j++) {
        for (int i = 0; i < g->n - 1; i++) {
            g->adjMatrix[i][j] = g->adjMatrix[i][j + 1];
        }
    }
    g->n--;
    printf("정점 %d 삭제 완료\n", v);
}

// ------------------------------
// 간선 추가 (양방향 그래프 기준)
// ------------------------------
void insertEdge(Graph* g, int v1, int v2) {
    int i1 = -1, i2 = -1;
    for (int i = 0; i < g->n; i++) {
        if (g->vertices[i] == v1) i1 = i;
        if (g->vertices[i] == v2) i2 = i;
    }
    if (i1 == -1 || i2 == -1) {
        printf("정점이 존재하지 않습니다.\n");
        return;
    }
    g->adjMatrix[i1][i2] = g->adjMatrix[i2][i1] = 1;
    printf("간선 (%d, %d) 추가 완료\n", v1, v2);
}

// ------------------------------
// 간선 삭제
// ------------------------------
void deleteEdge(Graph* g, int v1, int v2) {
    int i1 = -1, i2 = -1;
    for (int i = 0; i < g->n; i++) {
        if (g->vertices[i] == v1) i1 = i;
        if (g->vertices[i] == v2) i2 = i;
    }
    if (i1 == -1 || i2 == -1) {
        printf("정점이 존재하지 않습니다.\n");
        return;
    }
    g->adjMatrix[i1][i2] = g->adjMatrix[i2][i1] = 0;
    printf("간선 (%d, %d) 삭제 완료\n", v1, v2);
}

// ------------------------------
// 인접 정점 출력
// ------------------------------
void adjacentVertex(Graph* g, int v) {
    int idx = -1;
    for (int i = 0; i < g->n; i++) {
        if (g->vertices[i] == v) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("정점 %d를 찾을 수 없습니다.\n", v);
        return;
    }

    printf("정점 %d와 인접한 정점: ", v);
    int found = 0;
    for (int j = 0; j < g->n; j++) {
        if (g->adjMatrix[idx][j] == 1) {
            printf("%d ", g->vertices[j]);
            found = 1;
        }
    }
    if (!found) printf("없음");
    printf("\n");
}

// ------------------------------
// 전체 그래프 출력 (디버깅용)
// ------------------------------
void printGraph(Graph* g) {
    printf("\n[인접 행렬]\n    ");
    for (int i = 0; i < g->n; i++) printf("%3d", g->vertices[i]);
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("%3d", g->vertices[i]);
        for (int j = 0; j < g->n; j++) {
            printf("%3d", g->adjMatrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    Graph g;
    createGraph(&g);

    insertVertex(&g, 1);
    insertVertex(&g, 2);
    insertVertex(&g, 3);
    insertVertex(&g, 4);

    insertEdge(&g, 1, 2);
    insertEdge(&g, 1, 3);
    insertEdge(&g, 2, 4);

    printGraph(&g);

    printf("\n그래프가 비었는가? %s\n", isEmptyGraph(&g) ? "예" : "아니오");

    adjacentVertex(&g, 1);
    deleteEdge(&g, 1, 3);
    deleteVertex(&g, 2);

    printGraph(&g);
    return 0;
}