// Stack을 이용한 이진트리 판별

#include <stdio.h>

#include "linkedTree/linkedtree.h"

int main() {
	char input[1000];

	// 트리 입력받기
	if (fgets(input, sizeof(input), stdin) == NULL) {
		printf("ERROR\n");
		return 0;
	}

	Tree* tree;
	tree = createLinkedTree();
	makeLinkedTree(input);
	// makeArrayTree(input);
	 
	 
	
	//int result = isBinaryTree(tree, &BinarytreeFlag);

	// 이진트리 여부 출력
	if (result) printf("ERROR\n");
	else if (!BinarytreeFlag) printf("FALSE\n");
	else printf("TRUE\n");

	return 0;
}
