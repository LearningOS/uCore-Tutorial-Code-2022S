#include "queue.h"
#include "defs.h"

int process_queue_data[QUEUE_SIZE];

void init_queue(struct queue *q, int size, int *data)
{
	q->size = size;
	q->data = data;
	q->front = q->tail = 0;
	q->empty = 1;
}

void push_queue(struct queue *q, int value)
{
	if (!q->empty && q->front == q->tail) {
		panic("queue shouldn't be overflow");
	}
	q->empty = 0;
	q->data[q->tail] = value;
	q->tail = (q->tail + 1) % q->size;
}

int pop_queue(struct queue *q)
{
	if (q->empty)
		return -1;
	int value = q->data[q->front];
	q->front = (q->front + 1) % q->size;
	if (q->front == q->tail)
		q->empty = 1;
	return value;
}
