#pragma once

typedef struct TreeNode {
    char data;
    struct TreeNode* leftChild;
    struct TreeNode* rightSibling;
} TreeNode;

extern TreeNode* createNode(char data);
extern void addChild(TreeNode* parent, TreeNode* child);
extern int getHeight(TreeNode* root);
extern int getNodeCount(TreeNode* root);
extern int getLeafCount(TreeNode* root);
extern void deleteTree(TreeNode* root);
