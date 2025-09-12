// 1. Stack을 이용한 이진트리 판별
// 2. 재귀함수를 이용한 이진트리 판별

#include <stdio.h>

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

			// 자식이 2개 이상이라면 이진트리x
			if (childCount > 2) {
				destroyStack(stack);
				return FALSE;
			}

			// 자식이 2개 이하라면 다시 Stack에 저장
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
		if (tree[*index] == '(') { // 부모 노드 시작
			//printf("tree[%d] = %c\n", *index, tree[*index]);

			++(*index);
			if (!isBinaryTreeRec(tree, index)) return FALSE; // 자식 노드가 2개 이상일 시 FALSE
		}
		else if (tree[*index] == ')') { // 부모 노드 끝
			//printf("tree[%d] = %c\n", *index, tree[*index]);

			++(*index);
			return (childCount <= 2) ? TRUE : FALSE; // 자식 노드가 2개 이하라면 TRUE, 아니라면 FALSE
		}
		else {
			/*printf("tree[%d] = %c, childCount = %d\n",
				*index, tree[*index], childCount + 1);*/

			++(*index);
			++childCount; // 노드가 존재할 경우 증가
		}
		if (childCount > 2) return FALSE;
	}
	return TRUE; // 이진트리가 아니라면 TRUE
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
	//printf((isBinaryTreeRec(tree, &index) == TRUE) ? "TRUE\n" : "FALSE\n");

	return 0;
}
