// 이진트리의 높이, 노드 수, 단말 노드의 수를 출력

#include <stdio.h>

#include "linkedTree/linkedtree.h"

int main() {
	char input[1000];

	// 트리 입력받기
	fgets(input, sizeof(input), stdin);

	Tree* tree;
	tree = createLinkedTree();
	makeLinkedTree(input);

	return 0;
}
