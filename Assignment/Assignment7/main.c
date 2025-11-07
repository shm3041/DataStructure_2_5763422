#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define V 10
#define E 20
#define INF 9999

// 최소 거리 정점 선택
int minDistance(int dist[], int visited[]) {
    int min = INF, min_index = -1;
    for (int v = 0; v < V; v++) {
        if (!visited[v] && dist[v] < min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// 다익스트라 알고리즘
void dijkstra(int graph[V][V], int src, int result[V]) {
    int dist[V];
    int visited[V] = { 0 };

    for (int i = 0; i < V; i++)
        dist[i] = INF;

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, visited);
        if (u == -1) break; // 연결되지 않은 그래프 방지
        visited[u] = 1;

        for (int v = 0; v < V; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF
                && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }

    // 결과 복사
    for (int i = 0; i < V; i++)
        result[i] = dist[i];
}

// 무작위 그래프 생성
void generateRandomGraph(int graph[V][V]) {
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            graph[i][j] = 0;

    int edges = 0;
    while (edges < E) {
        int u = rand() % V;
        int v = rand() % V;
        int w = rand() % 20 + 1; // 가중치 1~20
        if (u != v && graph[u][v] == 0) {
            graph[u][v] = w;
            graph[v][u] = w;
            edges++;
        }
    }
}

// 출력 (A~J)
void printMatrix(int mat[V][V]) {
    printf("    ");
    for (int i = 0; i < V; i++) printf("%3c ", 'A' + i);
    printf("\n");
    for (int i = 0; i < V; i++) {
        printf("%3c ", 'A' + i);
        for (int j = 0; j < V; j++) {
            if (mat[i][j] == INF) printf("  ∞ ");
            else printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

int main() {
    srand((unsigned)time(NULL));

    int graph[V][V];
    generateRandomGraph(graph);

    printf("▶ 무작위 생성된 그래프 (가중치):\n");
    printMatrix(graph);

    int distAll[V][V];

    for (int i = 0; i < V; i++)
        dijkstra(graph, i, distAll[i]);

    printf("\n▶ 모든 노드 쌍 간 최단 거리:\n");
    printMatrix(distAll);

    return 0;
}
