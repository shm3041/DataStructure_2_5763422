#pragma once
#include <stddef.h>

typedef char TreeType; // Tree ��� Ÿ��

typedef struct {
	TreeType node[10000]; // Tree ��� ���� �迭(���� �迭)
	size_t size; // �迭 ���� ũ��
	int count; // ��� ����
} Tree;

extern void initTree(Tree* tree); // Ʈ�� �ʱ�ȭ
extern void buildTree(Tree* tree, const char* input); // Ʈ�� ���� (��ȣ -> �迭)
extern void preOrderTree(Tree* tree); // ���� ��ȸ
extern void inOrderTree(Tree* tree); // ���� ��ȸ
extern void postOrderTree(Tree* tree); // ���� ��ȸ

extern void trim(char* str); // ���ڿ� ���� ����
//extern void allocNode(Tree* tree); // ��� �迭 ���� �Ҵ�