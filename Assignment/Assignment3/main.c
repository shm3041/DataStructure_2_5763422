#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "tree.h"

int main() {
	Tree tree;
	initTree(&tree);

	char input[10000];
	scanf("%[^\n]", input);

	buildTree(&tree, input);

	printf("pre-order: ");
	preOrderTree(&tree);

	printf("in-order: ");
	inOrderTree(&tree);

	printf("post-order: ");
	postOrderTree(&tree);

	return 0;
}
