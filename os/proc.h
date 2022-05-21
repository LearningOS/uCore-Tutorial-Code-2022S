#ifndef PROC_H
#define PROC_H

#include "riscv.h"
#include "types.h"
#include "sync.h"

#define NPROC (128)
#define NTHREAD (16)
#define FD_BUFFER_SIZE (16)
#define LOCK_POOL_SIZE (8)

struct file;

// Saved registers for kernel context switches.
struct context {
	uint64 ra;
	uint64 sp;

	// callee-saved
	uint64 s0;
	uint64 s1;
	uint64 s2;
	uint64 s3;
	uint64 s4;
	uint64 s5;
	uint64 s6;
	uint64 s7;
	uint64 s8;
	uint64 s9;
	uint64 s10;
	uint64 s11;
};

enum threadstate { T_UNUSED, T_USED, SLEEPING, RUNNABLE, RUNNING, EXITED };
struct thread {
	enum threadstate state; // Thread state
	int tid; // Thread ID
	struct proc *process;
	uint64 ustack; // Virtual address of user stack
	uint64 kstack; // Virtual address of kernel stack
	struct trapframe *trapframe; // data page for trampoline.S
	struct context context; // swtch() here to run process
	uint64 exit_code;
};

enum procstate { P_UNUSED, P_USED, ZOMBIE };

// Per-process state
struct proc {
	enum procstate state; // Process state
	int pid; // Process ID
	pagetable_t pagetable; // User page table
	uint64 max_page;
	uint64 ustack_base; // Virtual address of user stack base
	struct proc *parent; // Parent process
	uint64 exit_code;
	//File descriptor table, using to record the files opened by the process
	struct file *files[FD_BUFFER_SIZE];
	struct thread threads[NTHREAD];
	// Use dummy increasing id as index index of lock pool because we don't have destroy method yet
	uint next_mutex_id, next_semaphore_id, next_condvar_id;
	struct mutex mutex_pool[LOCK_POOL_SIZE];
	struct semaphore semaphore_pool[LOCK_POOL_SIZE];
	struct condvar condvar_pool[LOCK_POOL_SIZE];
	// LAB5: (1) Define your variables for deadlock detect here.
	//			 You may need a flag to record if detection enabled,
	//       and some arrays for detection algorithm.
};

int cpuid();
struct proc *curr_proc();
struct thread *curr_thread(void);
void exit(int);
void proc_init();
void scheduler() __attribute__((noreturn));
void sched();
void yield();
int fork();
int exec(char *, char **);
int wait(int, int *);
void add_task(struct thread *);
struct thread *id_to_task(int);
int task_to_id(struct thread *);
struct thread *pop_task();
struct proc *allocproc();
int allocthread(struct proc *p, uint64 entry, int alloc_user_res);
uint64 get_thread_trapframe_va(int tid);
int fdalloc(struct file *);
int init_stdio(struct proc *);
int push_argv(struct proc *, char **);
// swtch.S
void swtch(struct context *, struct context *);

#endif // PROC_H