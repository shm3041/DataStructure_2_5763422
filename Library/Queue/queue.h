#pragma once

/*
Queue의 구현: Linked  List
1. Queue는 QueueNode 구조체의 link로 관리
2. QueueNode 구조체는 데이터 필드 data와 다음 노드를 가리키는 포인터 link로 구성
3. rear로 데이터 삽입, front로 추출하는 구조
4. link는 front에서 rear를 가리킴
*/

typedef int elementType;

typedef struct QueueNode {
	elementType data;
	struct QueueNode* link;
} QueueNode;

typedef struct Queue {
	QueueNode* rear;
	QueueNode* front;
	int follow;
} Queue;


// CRUD-PI
extern Queue* createQueue(void);
extern void destroyQueue(Queue* queue);

extern void enQueue(Queue* queue, elementType data);
extern elementType deQueue(Queue* queue);

extern void printQueue(Queue* queue);
extern void initQueue(Queue* queue);

extern int isEmptyQueue(Queue* queue);