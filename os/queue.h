#ifndef QUEUE_H
#define QUEUE_H
#define QUEUE_SIZE (1024)

// queue data for processing scheduling only
// for queue for wait queue of mutex/semaphore/condvar, provide other data
extern int process_queue_data[QUEUE_SIZE];

struct queue {
	int *data;
	int size;
	int front;
	int tail;
	int empty;
};

void init_queue(struct queue *, int, int *);
void push_queue(struct queue *, int);
int pop_queue(struct queue *);

#endif // QUEUE_H
