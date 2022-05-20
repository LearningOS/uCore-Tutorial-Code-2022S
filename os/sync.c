#include "defs.h"
#include "proc.h"
#include "sync.h"

struct mutex *mutex_create(int blocking)
{
	struct proc *p = curr_proc();
	if (p->next_mutex_id >= LOCK_POOL_SIZE) {
		return NULL;
	}
	struct mutex *m = &p->mutex_pool[p->next_mutex_id];
	p->next_mutex_id++;
	m->blocking = blocking;
	m->locked = 0;
	if (blocking) {
		// blocking mutex need wait queue but spinning mutex not
		init_queue(&m->wait_queue, WAIT_QUEUE_MAX_LENGTH,
			   m->_wait_queue_data);
	}
	return m;
}

void mutex_lock(struct mutex *m)
{
	if (!m->locked) {
		m->locked = 1;
		debugf("lock a free mutex");
		return;
	}
	if (!m->blocking) {
		// spin mutex will just poll
		debugf("try to lock spin mutex");
		while (m->locked) {
			yield();
		}
		debugf("lock spin mutex after some trials");
		return;
	}
	// blocking mutex will wait in the queue
	struct thread *t = curr_thread();
	push_queue(&m->wait_queue, task_to_id(t));
	// don't forget to change thread state to SLEEPING
	t->state = SLEEPING;
	debugf("block to wait for mutex");
	sched();
	debugf("blocking mutex passed to me");
	// here lock is released (with locked = 1) and passed to me, so just do nothing
}

void mutex_unlock(struct mutex *m)
{
	if (m->blocking) {
		struct thread *t = id_to_task(pop_queue(&m->wait_queue));
		if (t == NULL) {
			// Without waiting thread, just release the lock
			m->locked = 0;
			debugf("blocking mutex released");
		} else {
			// Or we should give lock to next thread
			t->state = RUNNABLE;
			add_task(t);
			debugf("blocking mutex passed to thread %d", t->tid);
		}
	} else {
		m->locked = 0;
		debugf("spin mutex unlocked");
	}
}

struct semaphore *semaphore_create(int count)
{
	struct proc *p = curr_proc();
	if (p->next_semaphore_id >= LOCK_POOL_SIZE) {
		return NULL;
	}
	struct semaphore *s = &p->semaphore_pool[p->next_semaphore_id];
	p->next_semaphore_id++;
	s->count = count;
	init_queue(&s->wait_queue, WAIT_QUEUE_MAX_LENGTH, s->_wait_queue_data);
	return s;
}

void semaphore_up(struct semaphore *s)
{
	s->count++;
	if (s->count <= 0) {
		// count <= 0 after up means wait queue not empty
		struct thread *t = id_to_task(pop_queue(&s->wait_queue));
		if (t == NULL) {
			panic("count <= 0 after up but wait queue is empty?");
		}
		t->state = RUNNABLE;
		add_task(t);
		debugf("semaphore up and notify another task");
	}
	debugf("semaphore up from %d to %d", s->count - 1, s->count);
}

void semaphore_down(struct semaphore *s)
{
	s->count--;
	if (s->count < 0) {
		// s->count < 0 means need to wait (state=SLEEPING)
		struct thread *t = curr_thread();
		push_queue(&s->wait_queue, task_to_id(t));
		t->state = SLEEPING;
		debugf("semaphore down to %d and wait...", s->count);
		sched();
		debugf("semaphore up to %d and wake up", s->count);
	}
	debugf("finish semaphore_down with count = %d", s->count);
}

struct condvar *condvar_create()
{
	struct proc *p = curr_proc();
	if (p->next_condvar_id >= LOCK_POOL_SIZE) {
		return NULL;
	}
	struct condvar *c = &p->condvar_pool[p->next_condvar_id];
	p->next_condvar_id++;
	init_queue(&c->wait_queue, WAIT_QUEUE_MAX_LENGTH, c->_wait_queue_data);
	return c;
}

void cond_signal(struct condvar *cond)
{
	struct thread *t = id_to_task(pop_queue(&cond->wait_queue));
	if (t) {
		t->state = RUNNABLE;
		add_task(t);
		debugf("signal wake up thread %d", t->tid);
	} else {
		debugf("dummpy signal");
	}
}

void cond_wait(struct condvar *cond, struct mutex *m)
{
	// conditional variable will unlock the mutex first and lock it again on return
	mutex_unlock(m);
	struct thread *t = curr_thread();
	// now just wait for cond
	push_queue(&cond->wait_queue, task_to_id(t));
	t->state = SLEEPING;
	debugf("wait for cond");
	sched();
	debugf("wake up from cond");
	mutex_lock(m);
}
