#pragma once

/*
Queue�� ����: Linked  List
1. Queue�� QueueNode ����ü�� link�� ����
2. QueueNode ����ü�� ������ �ʵ� data�� ���� ��带 ����Ű�� ������ link�� ����
3. rear�� ������ ����, front�� �����ϴ� ����
4. link�� front���� rear�� ����Ŵ
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