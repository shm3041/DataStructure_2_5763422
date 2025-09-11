// Stack을 이용한 이진트리 판별

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "stack/stack.h"

#define TRUE 1
#define FALSE 0

void trim(char* str) {
	char* src = str;
	char* dst = str;
	while (*src) {
		if (*src != ' ' && *src != '\n') *dst++ = *src;
		++src;
	}
	*dst = '\0';
}

// 이진트리 판별 (재귀 X)
int isBinaryTree(const char* tree) {
	Stack* stack = createStack();

	for (int i = 0; tree[i] != '\0'; ++i) {
		if (tree[i] == '(') push(stack, 0); // 부모 노드 시작
		else if (tree[i] == ')') pop(stack); // 부모 노드 종료
		else { // 노드가 존재할 시 
			// stack에서 자식 개수를 가져온 후, 1증가
			elementType childCount = pop(stack);
			++childCount;

			// 이 값이 2보다 클 경우 이진트리가 아님
			if (childCount > 2) {
				destroyStack(stack);
				return FALSE;
			}

			// 만약 작거나 같을 시 이진트리 조건에 부합하므로 childCout를
			// stack에 다시 저장
			push(stack, childCount);
		}
//#ifdef _DEBUG
//		printf("tree[%d] = %c\n", i, tree[i]);
//		printStack(stack);
//#endif
	}
	destroyStack(stack);
	return TRUE;
}

// 이진트리 판별 (재귀 O)
int isBinaryTreeRec(const char* tree, int* index) {
	int childCount = 0;

	while (tree[*index] != '\0') {
		if (tree[*index] == '(') {
			++(*index);
			if (!isBinaryTreeRec(tree, index)) return FALSE; // 하위 트리 검사
		}
		else if (tree[*index] == ')') {
			++(*index);
			return (childCount <= 2) ? TRUE : FALSE;
		}
		else {
			++(*index);
			++childCount;
		}
		//if (childCount > 2) return FALSE;
	}

	return (childCount <= 2) ? TRUE : FALSE;
}

int main() {
	char tree[1000];

	// 트리 입력받기
	fgets(tree, sizeof(tree), stdin);

	// 공백, 개행문자 제거
	trim(tree);

	// 이진트리 여부 출력
	int index = 0;
	printf((isBinaryTree(tree) == TRUE) ? "TRUE\n" : "FALSE\n");
	printf((isBinaryTreeRec(tree, &index) == TRUE) ? "TRUE\n" : "FALSE\n");

	return 0;
}
