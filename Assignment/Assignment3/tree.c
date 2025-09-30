#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tree.h"
#include "stack.h"

void trim(char* str) {
	char* src = str;
	char* dst = str;
 	while (*src) {
		if (*src != ' ' && *src != '\n') *dst++ = *src;
		++src;
	}
	*dst = '\0';
}

void initTree(Tree* tree) {
	tree->size = 0;
	tree->count = 0;

	for (size_t i = 0; i < 10000; i++) tree->node[i] = '\0';
}

void buildTree(Tree* tree, const char* input) {
	if (!input) return;

	char buf[10000];
	strncpy(buf, input, 9999);
	buf[9999] = '\0';
	trim(buf);
	initTree(tree);

	Stack* stack = createStack();
	int index = 1;

	for (size_t i = 0; buf[i]; i++) {
		char ch = buf[i];
		if (ch == '(') {
			elementType item = { index, 0 };
			push(stack, item);
			index = -1;
		}
		else if (ch == ')') {
			if (!isEmptyStack(stack)) {
				elementType item = pop(stack);
				index = item.parentIndex;
			}
		}
		else if (isalpha(ch)) {
			if (index == -1) index = 1; // 루트
			tree->node[index] = ch;
			if (index > tree->count) tree->count = index;

			if (!isEmptyStack(stack)) {
				elementType item = pop(stack);
				if (item.childDir == 0) {
					// 왼쪽 자식 완료 → 오른쪽 준비
					elementType rightItem = { item.parentIndex, 1 };
					push(stack, rightItem);
					index = 2 * item.parentIndex; // 왼쪽
				}
				else {
					index = 2 * item.parentIndex + 1; // 오른쪽
				}
			}
		}
	}

	destroyStack(stack);
}

void preOrderTree(Tree* tree) {
	if (tree->count == 0) return;
	Stack* stack = createStack();
	push(stack, (elementType) { 1, 0 });
	while (!isEmptyStack(stack)) {
		elementType item = pop(stack);
		int idx = item.parentIndex;
		if (idx > tree->count || tree->node[idx] == 0) continue;
		printf("%c ", tree->node[idx]);
		push(stack, (elementType) { 2 * idx + 1, 0 }); // 오른쪽
		push(stack, (elementType) { 2 * idx, 0 });   // 왼쪽
	}
	printf("\n");
	destroyStack(stack);
}

void inOrderTree(Tree* tree) {
	if (tree->count == 0) return;
	Stack* stack = createStack();
	int idx = 1;
	while (1) {
		while (idx <= tree->count && tree->node[idx] != 0) {
			push(stack, (elementType) { idx, 0 });
			idx = 2 * idx;
		}
		if (isEmptyStack(stack)) break;
		elementType item = pop(stack);
		idx = item.parentIndex;
		printf("%c ", tree->node[idx]);
		idx = 2 * idx + 1;
	}
	printf("\n");
	destroyStack(stack);
}

void postOrderTree(Tree* tree) {
	if (tree->count == 0) return;
	Stack* stack1 = createStack();
	Stack* stack2 = createStack();
	push(stack1, (elementType) { 1, 0 });
	while (!isEmptyStack(stack1)) {
		elementType item = pop(stack1);
		int idx = item.parentIndex;
		if (idx > tree->count || tree->node[idx] == 0) continue;
		push(stack2, (elementType) { idx, 0 });
		push(stack1, (elementType) { 2 * idx, 0 });
		push(stack1, (elementType) { 2 * idx + 1, 0 });
	}
	while (!isEmptyStack(stack2)) {
		elementType item = pop(stack2);
		printf("%c ", tree->node[item.parentIndex]);
	}
	printf("\n");
	destroyStack(stack1);
	destroyStack(stack2);
}