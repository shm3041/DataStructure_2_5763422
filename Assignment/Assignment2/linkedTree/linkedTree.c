#include <stdlib.h>
#include "linkedTree.h"

TreeNode* createNode(char data) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->data = data;
    node->leftChild = NULL;
    node->rightSibling = NULL;
    return node;
}

void addChild(TreeNode* parent, TreeNode* child) {
    if (!parent || !child) return;
    if (!parent->leftChild)
        parent->leftChild = child;
    else {
        TreeNode* curr = parent->leftChild;
        while (curr->rightSibling)
            curr = curr->rightSibling;
        curr->rightSibling = child;
    }
}

int getHeight(TreeNode* root) {
    if (!root) return -1;
    int maxH = -1;
    for (TreeNode* c = root->leftChild; c; c = c->rightSibling) {
        int h = getHeight(c);
        if (h > maxH) maxH = h;
    }
    return maxH + 1;
}

int getNodeCount(TreeNode* root) {
    if (!root) return 0;
    int cnt = 1;
    for (TreeNode* c = root->leftChild; c; c = c->rightSibling)
        cnt += getNodeCount(c);
    return cnt;
}

int getLeafCount(TreeNode* root) {
    if (!root) return 0;
    if (!root->leftChild) return 1;
    int cnt = 0;
    for (TreeNode* c = root->leftChild; c; c = c->rightSibling)
        cnt += getLeafCount(c);
    return cnt;
}

void deleteTree(TreeNode* root) {
    if (!root) return;
    TreeNode* c = root->leftChild;
    while (c) {
        TreeNode* next = c->rightSibling;
        deleteTree(c);
        c = next;
    }
    free(root);
}
