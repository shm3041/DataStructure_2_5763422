#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

Stack* createStack() {
	// 스택 생성, stack의 top을 NULL로 초기화
	Stack* stack;

	stack = (Stack*)malloc(sizeof(Stack));
	stack->top = NULL;

	return stack;
}

void destroyStack(Stack* stack) {
	// 만약 스택이 존재한다면 초기화 후 메모리 해제
	if (stack == NULL) return;
	initStack(stack);
	free(stack);
	return;
}

void initStack(Stack* stack) {
	// 스택이 비어있다면 종료
	// 스택에 블록이 존재한다면, pop을 통해 모든 블록 삭제
	if (stack->top == NULL) return;
	while (!isEmptyStack(stack)) {
		pop(stack);
	}

	return;
}

void push(Stack* stack, elementType item) {
	// 스택 블록 생성, 데이터를 삽입 후 newNode에 기존 노드 연결
	// top을 newNode로 변경
	StackNode* newNode;

	newNode = (StackNode*)malloc(sizeof(StackNode));
	newNode->data = item;
	newNode->link = stack->top;
	stack->top = newNode;

	return;
}

elementType pop(Stack* stack) {
	// 만약 스택이 비어있지 않다면 top이 가리키는 블록을 삭제,
	// 아래 블록을 top으로 변경, pop한 데이터 반환
	if (isEmptyStack(stack)) {
		printf("Stack is empty\n");
		return NULL;
	}

	StackNode* temp = stack->top;
	elementType item = temp->data;

	stack->top = temp->link;

	free(temp);
	return item;
}

int isEmptyStack(Stack* stack) {
	// 스택이 비어있다면 1, 아니면 0 반환
	if (stack->top == NULL) return 1;
	else return 0;
}

void printStack(Stack* stack) {
	// 스택이 비어있지 않다면, Stack에 있는 모든 데이터를 출력
	if (isEmptyStack(stack)) {
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