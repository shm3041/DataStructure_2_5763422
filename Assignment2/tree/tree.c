#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"
#include "../stack/stack.h"

Tree* createTree(void) {
	Tree* tree;
	tree = (Tree*)malloc(sizeof(Tree));
	tree->node = NULL;

	return tree;
}

void destroyTree(Tree* tree) {
	if (isEmpty(tree)) free(tree);
}

Tree* makeTree(const char* tree) {
	
}


int isEmpty(Tree* tree) {
	if (tree == NULL) return 1; // true
	else return 0; // false
}

void eraseSpaceEol(char* str) {
	char* src = str;
	char* dst = str;
	while (*src) {
		if (*src != ' ' && *src != '\n') *dst++ = *src;
		++src;
	}
	*dst = '\0';
}

// ����Ʈ�� �Ǻ�
int isBinaryTree(char* tree, int* BinarytreeFlag) {
	eraseSpaceEol(tree);

	if (tree == NULL || tree[0] == '\0') return -1; // �� ���ڿ�

	if (tree[0] != '(') return -1; // ��ȣ ���� X

	Stack* stack = createStack();
	int rootCount = 0;
	int alphaFlag = 0;
	for (int i = 0; tree[i] != '\0'; ++i) {
		if (tree[i] == '(') push(stack, 0); // �θ� ��� ����

		else if (tree[i] == ')') { // �θ� ��� ����
			if (isEmpty(stack)) { // ��ȣ ���� �ʰ�
				destroyStack(stack);
				return -1;
			}

			pop(stack);
		}
		else if (isalpha((unsigned char)tree[i])) { // Ʈ�� ��尡 ���ĺ��� ���
			if (isEmpty(stack)) { // ��ȣ �ܺο� ��尡 �����ϴ� ���
				destroyStack(stack);
				return -1;
			}

			alphaFlag = 1; // ��� ���翩�� Ȯ��

			// stack���� �ڽ� ������ ������ ��, 1����
			elementType childCount = pop(stack);
			childCount++;

			// �� ���� 2���� Ŭ ��� ����Ʈ���� �ƴ�
			if (childCount > 2) {
				*BinarytreeFlag = 0;
			}

			// ���� �۰ų� ���� �� ����Ʈ�� ���ǿ� �����ϹǷ� childCout��
			// stack�� �ٽ� ����
			push(stack, childCount);
		}
		else { // ���ĺ�, '(', ')' ���� ���ڰ� ����
			destroyStack(stack);
			return -1;
		}
	}

	if (!isEmpty(stack)) { // ��ȣ ���� ����
		destroyStack(stack);
		return -1;
	}

	if (!alphaFlag) { // ��尡 �������� ����
		destroyStack(stack);
		return -1;
	}

	destroyStack(stack);
	return 0;
}