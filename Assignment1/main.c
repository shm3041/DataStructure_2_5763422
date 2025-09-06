// Stack을 이용한 이진트리 판별

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "stack/stack.h"

void eraseSpaceEol(char* str) {
	char* src = str;
	char* dst = str;
	while (*src) {
		if (*src != ' ' && *src != '\n') *dst++ = *src;
		++src;
	}
	*dst = '\0';
}

// 이진트리 판별 (Stack만 사용, 재귀 없음)
int isBinaryTree(const char* tree) {
	Stack* stack = createStack();
	elementType childCount = 0;

	for (int i = 0; tree[i] != '\0'; ++i) {
		if (tree[i] == '(') { // 부모 노드 시작
			childCount = 0;
			push(stack, childCount);
		}
		else if (tree[i] == ')') { // 부모 노드 종료
			pop(stack);
		}
		else {
			// stack에서 자식 개수를 가져온 후, 1증가
			childCount = pop(stack);
			childCount++;

			// 이 값이 2보다 클 경우 이진트리가 아님
			if (childCount > 2) {
				destroyStack(stack);
				return -1;
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
	return 0;
}

int main() {
	char tree[1000];

	// 트리 입력받기
	fgets(tree, sizeof(tree), stdin);
	// 공백, 개행문자 제거
	eraseSpaceEol(tree);

//#ifdef _DEBUG
//	printf("input: %s\n", tree);
//#endif

	// 이진트리 여부 출력
	printf("%s\n", isBinaryTree(tree) == 0 ? "TRUE" : "FALSE");

	return 0;
}
