#pragma once

/*
stack의 구현: Linked List
1. stack은 stackNode 구조체의 포인터 top을 통해 관리
2. stackNode 구조체는 데이터 필드 data와 다음 노드를 가리키는 포인터 link로 구성
*/

typedef int elementType; // 데이터의 타입 정의

typedef struct StackNode {
	elementType data; // 데이터 필드
	struct StackNode* link; // 다음 노드를 가리키는 포인터
} StackNode;

typedef struct Stack {
	StackNode* top; // stack 블록
} Stack;

// function CRUD-PI
extern Stack* createStack(void);
extern void destroyStack(Stack* stack);
extern void initStack(Stack* stack);

extern void push(Stack* stack, elementType item);
extern elementType pop(Stack* stack);

extern int isEmptyStack(Stack* stack);

extern void printStack(Stack* stack);
