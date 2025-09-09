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

// 이진트리 판별
int isBinaryTree(char* tree, int* BinarytreeFlag) {
	eraseSpaceEol(tree);

	if (tree == NULL || tree[0] == '\0') return -1; // 빈 문자열

	if (tree[0] != '(') return -1; // 괄호 시작 X

	Stack* stack = createStack();
	int rootCount = 0;
	int alphaFlag = 0;
	for (int i = 0; tree[i] != '\0'; ++i) {
		if (tree[i] == '(') push(stack, 0); // 부모 노드 시작

		else if (tree[i] == ')') { // 부모 노드 종료
			if (isEmpty(stack)) { // 괄호 개수 초과
				destroyStack(stack);
				return -1;
			}

			pop(stack);
		}
		else if (isalpha((unsigned char)tree[i])) { // 트리 노드가 알파벳인 경우
			if (isEmpty(stack)) { // 괄호 외부에 노드가 존재하는 경우
				destroyStack(stack);
				return -1;
			}

			alphaFlag = 1; // 노드 존재여부 확인

			// stack에서 자식 개수를 가져온 후, 1증가
			elementType childCount = pop(stack);
			childCount++;

			// 이 값이 2보다 클 경우 이진트리가 아님
			if (childCount > 2) {
				*BinarytreeFlag = 0;
			}

			// 만약 작거나 같을 시 이진트리 조건에 부합하므로 childCout를
			// stack에 다시 저장
			push(stack, childCount);
		}
		else { // 알파벳, '(', ')' 외의 문자가 존재
			destroyStack(stack);
			return -1;
		}
	}

	if (!isEmpty(stack)) { // 괄호 개수 부족
		destroyStack(stack);
		return -1;
	}

	if (!alphaFlag) { // 노드가 존재하지 않음
		destroyStack(stack);
		return -1;
	}

	destroyStack(stack);
	return 0;
}