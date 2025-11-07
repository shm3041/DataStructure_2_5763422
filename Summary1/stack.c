#include "stack.h"

short isFull(Stack* s) {
	if (s->top == (S_SIZE - 1)) return 1;
	return 0;
}

short isEmpty(Stack* s) {
	if (s->top == -1) return 1;
	return 0;
}

void initStack(Stack* s) {
	s->top = -1;
}

void pushStack(Stack* s, SElement e) {
	if (isFull(s)) return;
	s->data[++(s->top)] = e;
}

SElement popStack(Stack* s) {
	if (isEmpty(s)) return S_NULL;
	return s->data[(s->top)--];
}

SElement peekStack(Stack* s) {
	if (isEmpty(s)) return S_NULL;
	return s->data[s->top];
}