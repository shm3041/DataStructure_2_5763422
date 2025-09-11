#pragma once

/*
stack�� ����: Linked List
1. stack�� stackNode ����ü�� ������ top�� ���� ����
2. stackNode ����ü�� ������ �ʵ� data�� ���� ��带 ����Ű�� ������ link�� ����
*/

typedef int elementType; // �������� Ÿ�� ����

typedef struct StackNode {
	elementType data; // ������ �ʵ�
	struct StackNode* link; // ���� ��带 ����Ű�� ������
} StackNode;

typedef struct Stack {
	StackNode* top; // stack ���
} Stack;

// function CRUD-PI
extern Stack* createStack(void);
extern void destroyStack(Stack* stack);
extern void initStack(Stack* stack);

extern void push(Stack* stack, elementType item);
extern elementType pop(Stack* stack);

extern int isEmptyStack(Stack* stack);

extern void printStack(Stack* stack);
