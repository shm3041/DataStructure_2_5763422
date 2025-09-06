#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

Stack* createStack() {
	// ���� ����, stack�� top�� NULL�� �ʱ�ȭ
	Stack* stack;

	stack = (Stack*)malloc(sizeof(Stack));
	stack->top = NULL;

	return stack;
}

void destroyStack(Stack* stack) {
	// ���� ������ �����Ѵٸ� �ʱ�ȭ �� �޸� ����
	if (stack == NULL) return;
	initStack(stack);
	free(stack);
	return;
}

void initStack(Stack* stack) {
	// ������ ����ִٸ� ����
	// ���ÿ� ����� �����Ѵٸ�, pop�� ���� ��� ��� ����
	if (stack->top == NULL) return;
	while (!isEmpty(stack)) {
		pop(stack);
	}

	return;
}

void push(Stack* stack, elementType item) {
	// ���� ��� ����, �����͸� ���� �� newNode�� ���� ��� ����
	// top�� newNode�� ����
	StackNode* newNode;

	newNode = (StackNode*)malloc(sizeof(StackNode));
	newNode->data = item;
	newNode->link = stack->top;
	stack->top = newNode;

	return;
}

elementType pop(Stack* stack) {
	// ���� ������ ������� �ʴٸ� top�� ����Ű�� ����� ����,
	// �Ʒ� ����� top���� ����, pop�� ������ ��ȯ
	if (isEmpty(stack)) {
		printf("Stack is empty\n");
		return NULL;
	}

	StackNode* temp = stack->top;
	elementType item = temp->data;

	stack->top = stack->top->link;

	free(temp);
	return item;
}

int isEmpty(Stack* stack) {
	// ������ ����ִٸ� 1, �ƴϸ� 0 ��ȯ
	if (stack->top == NULL) return 1;
	else return 0;
}

void printStack(Stack* stack) {
	// ������ ������� �ʴٸ�, Stack�� �ִ� ��� �����͸� ���
	if (isEmpty(stack)) {
		printf("Stack is empty\n");
		return;
	}

	StackNode* node = stack->top;

	printf("Stack: ");
	while (node != NULL) {
		printf("%s ", node->data);
		node = node->link;
	}
	printf("\n");
	return;
}