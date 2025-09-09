// Stack을 이용한 이진트리 판별

#include <stdio.h>

#include "tree/tree.h"

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
	int BinarytreeFlag = 1;
	int result = isBinaryTree(tree, &BinarytreeFlag);

	// 이진트리 여부 출력
	if (result) printf("ERROR\n");
	else if (!BinarytreeFlag) printf("FALSE\n");
	else printf("TRUE\n");

	return 0;
}
