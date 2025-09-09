#pragma once

typedef int elementType;

typedef struct Node {
	elementType data;
	Node*leftNode, *rightNode;
} Node;

typedef struct Tree {
	Node* node;
} Tree;

//typedef int* array;

// CRUD-PI

//Create, Destroy
extern Tree* createTree(void);
extern void destroyTree(Tree* tree);

// Update, Read
extern Tree* makeTree(const char* tree);
extern Node* getRoot(Tree* tree);
extern Node* getleftChild(Tree* tree);
extern Node* getrightChild(Tree* tree);

// Print, Init
extern void printTree(Tree* tree);
extern void initTree(Tree* tree);


extern int isEmpty(Tree* tree);

extern void traversal(Tree* tree, const char* order); // ¼øÈ¸