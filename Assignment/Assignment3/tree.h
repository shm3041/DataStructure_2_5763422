#pragma once
#include <stddef.h>

typedef char TreeType; // Tree 노드 타입

typedef struct {
	TreeType node[10000]; // Tree 노드 저장 배열(가변 배열)
	size_t size; // 배열 현재 크기
	int count; // 노드 개수
} Tree;

extern void initTree(Tree* tree); // 트리 초기화
extern void buildTree(Tree* tree, const char* input); // 트리 생성 (괄호 -> 배열)
extern void preOrderTree(Tree* tree); // 전위 순회
extern void inOrderTree(Tree* tree); // 중위 순회
extern void postOrderTree(Tree* tree); // 후위 순회

extern void trim(char* str); // 문자열 공백 제거
//extern void allocNode(Tree* tree); // 노드 배열 동적 할당