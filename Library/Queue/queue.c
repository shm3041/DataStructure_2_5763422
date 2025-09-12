#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

Queue* createQueue(void) {
	// ť ����
	Queue* queue;

	queue = (Queue*)malloc(sizeof(Queue));
	queue->rear = NULL; queue->front = NULL;
	queue->follow = 0;

	return queue;
}

void destroyQueue(Queue* queue) {
	// ť ����
	initQueue(queue);
	free(queue);
	return;
}

void enQueue(Queue* queue, elementType data) {
	// ������ ����
	// rear�� ����, front�� ����
	QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));

	newNode->data = data;
	newNode->link = NULL;
	++(queue->follow);

	if (isEmptyQueue(queue)) {
		queue->rear = newNode;
		queue->front = newNode;
	}
	else {
		// front���⿡�� rear������ ����Ű�� ����
		queue->rear->link = newNode;
		queue->rear = newNode;
	}
	return;
}

elementType deQueue(Queue* queue) {
	// ������ ����
	if (isEmptyQueue(queue)) {
		printf("Queue Empty!\n");
		return 0;
	}

	QueueNode* frontNode = queue->front;
	elementType data = frontNode->data;

	queue->front = queue->front->link;
	--(queue->follow);
	
	free(frontNode);
	return data;
}

void printQueue(Queue* queue) {
	// ť�� �ִ� �����͸� ��� ���
	if (isEmptyQueue(queue)) {
		printf("Queue Empty!\n");
		return;
	}

	QueueNode* printNode = queue->front;

	for (int i = 0; i < queue->follow; ++i) {
		printf("[%d] %d\n", i, printNode->data);
		printNode = printNode->link;
	}
}

void initQueue(Queue* queue) {
	// ť�� �ִ� �����͸� ��� ����
	if (isEmptyQueue(queue)) return;

	while (queue->front != NULL) {
		QueueNode* delNode = queue->front;
		queue->front = queue->front->link;
		free(delNode);
	}

	queue->rear = NULL; queue->front = NULL;
	queue->follow = 0;
	return;
}

int isEmptyQueue(Queue* queue) {
	if (queue->front == NULL) return 1;
	return 0;
}
