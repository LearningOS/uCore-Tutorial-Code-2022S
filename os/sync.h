#ifndef SYNC_H
#define SYNC_H
#include "queue.h"
#include "types.h"

#define WAIT_QUEUE_MAX_LENGTH 16

struct mutex {
	uint blocking;
	uint locked;
	struct queue wait_queue;
	// "alloc" data for wait queue
	int _wait_queue_data[WAIT_QUEUE_MAX_LENGTH];
};

struct semaphore {
	int count;
	struct queue wait_queue;
	// "alloc" data for wait queue
	int _wait_queue_data[WAIT_QUEUE_MAX_LENGTH];
};

struct condvar {
	struct queue wait_queue;
	// "alloc" data for wait queue
	int _wait_queue_data[WAIT_QUEUE_MAX_LENGTH];
};

struct mutex *mutex_create(int blocking);
void mutex_lock(struct mutex *);
void mutex_unlock(struct mutex *);
struct semaphore *semaphore_create(int count);
void semaphore_up(struct semaphore *);
void semaphore_down(struct semaphore *);
struct condvar *condvar_create();
void cond_signal(struct condvar *);
void cond_wait(struct condvar *, struct mutex *);
#endif
