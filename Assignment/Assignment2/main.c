#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include "linkedTree/linkedTree.h"
#include "stack/stack.h"

TreeNode* buildTreeFromString(const char* str) {
    Stack* st = createStack();
    TreeNode* root = NULL;
    TreeNode* currParent = NULL;
    TreeNode* lastAdded = NULL;

    for (int i = 0; str[i]; i++) {
        char ch = str[i];
        if (ch == '(') {
            push(st, currParent);
            currParent = lastAdded; // '(' 다음에는 마지막 추가 노드가 부모가 됨
            lastAdded = NULL;
        }
        else if (ch == ')') {
            currParent = (TreeNode*)pop(st);
            lastAdded = NULL;
        }
        else if (isspace(ch)) {
            continue;
        }
        else {
            TreeNode* newNode = createNode(ch);
            if (!root) {
                root = newNode;
            }
            else {
                if (!currParent->leftChild) {
                    currParent->leftChild = newNode;
                }
                else {
                    TreeNode* temp = currParent->leftChild;
                    while (temp->rightSibling) temp = temp->rightSibling;
                    temp->rightSibling = newNode;
                }
            }
            lastAdded = newNode;
        }
    }

    destroyStack(st);
    return root;
}

int main() {
    char input[512];
    scanf("%[^\n]", input);

    TreeNode* root = buildTreeFromString(input);

    int h = getHeight(root);
    if (h < 0) h = 0;
    int n = getNodeCount(root);
    int l = getLeafCount(root);

    printf("%d,%d,%d\n", h, n, l);

    deleteTree(root);
    return 0;
}
