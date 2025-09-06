#include <stdio.h>
#include "stack.h"

int main() {
	Stack* stack = createStack();

	printf("Is stack empty? %s\n", isEmpty(stack) ? "Yes" : "No");

	push(stack, "first");
	push(stack, "second");
	push(stack, "third");
	printStack(stack);

	printf("Popped: %s\n", pop(stack));
	printStack(stack);

	initStack(stack);
	printStack(stack);

	destroyStack(stack);

	return 0;
}