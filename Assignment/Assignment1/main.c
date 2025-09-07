// Stack을 이용한 이진트리 판별

#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
int isBinaryTree(const char* tree, int* BinarytreeFlag) {
	if (tree == NULL || tree[0] == '\0') { // 빈 문자열
		return -1;
	}

	Stack* stack = createStack();
	elementType childCount = 0;

	for (int i = 0; tree[i] != '\0'; ++i) {
		if (tree[i] == '(') { // 부모 노드 시작
			childCount = 0;
			push(stack, childCount);
		}
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

			// stack에서 자식 개수를 가져온 후, 1증가
			childCount = pop(stack);
			childCount++;

			// 이 값이 2보다 클 경우 이진트리가 아님
			if (childCount > 2) {
				//destroyStack(stack);
				//return 2;
				*BinarytreeFlag = 1;
			}

			// 만약 작거나 같을 시 이진트리 조건에 부합하므로 childCout를
			// stack에 다시 저장
			push(stack, childCount);
		}
		else { // 알파벳, '(', ')' 외의 문자가 존재
			destroyStack(stack);
			return -1;
		}

//#ifdef _DEBUG
//		printf("tree[%d] = %c\n", i, tree[i]);
//		printStack(stack);
//#endif
	}

	if (!isEmpty(stack)) { // 괄호 개수 부족
		destroyStack(stack);
		return -1;
	}

	destroyStack(stack);
	return 0;
}

int main() {
	char tree[1000];

	// 트리 입력받기
	if (fgets(tree, sizeof(tree), stdin) == NULL) {
		printf("ERROR\n");
		return 0;
	}

	// 공백, 개행문자 제거
	eraseSpaceEol(tree);

	// 이진트리 여부 확인
	int BinarytreeFlag = 0;
	int result = isBinaryTree(tree, &BinarytreeFlag);
	
//#ifdef _DEBUG
//	printf("input: %s\n", tree);
// 	printf("%d, %d\n", BinarytreeFlag, result);
//#endif

	// 이진트리 여부 출력
	if (result) printf("ERROR\n");
	else if (BinarytreeFlag) printf("FALSE\n");
	else printf("TRUE\n");

	return 0;
}
