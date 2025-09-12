#include <stdio.h>

#include "queue.h"

int main() {
	Queue* queue;
	queue = createQueue();

	enQueue(queue, 10);
	enQueue(queue, 20);
	enQueue(queue, 30);
	enQueue(queue, 40);
	printQueue(queue);

	int dQ;
	dQ = deQueue(queue);
	if (dQ) printf("deQueue: %d\n", dQ);
	dQ = deQueue(queue);
	if (dQ) printf("deQueue: %d\n", dQ);
	dQ = deQueue(queue);
	if (dQ) printf("deQueue: %d\n", dQ);
	printQueue(queue);

	initQueue(queue);
	enQueue(queue, 100);
	printQueue(queue);

	destroyQueue(queue);

	return 0;
}