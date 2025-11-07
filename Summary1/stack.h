#pragma once

#define S_SIZE 1000
#define S_NULL _Generic((SElement)0, \
    int: 0, \
    double: 0.0, \
    char*: NULL, \
    default: 0)

#include <stddef.h>

typedef int SElement;

typedef struct Stack {
	SElement data[S_SIZE]; // data
	int top; // index
} Stack;

extern void initStack(Stack* s);
extern void pushStack(Stack* s, SElement e);
extern SElement popStack(Stack* s);
extern SElement peekStack(Stack* s);
// isFull, isEmpty는 stack.c에서만 사용.
// 따라서 extern 선언 x