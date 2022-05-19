#include "loader.h"
#include "defs.h"
#include "file.h"
#include "trap.h"

extern char INIT_PROC[];

int bin_loader(struct inode *ip, struct proc *p)
{
	ivalid(ip);
	void *page;
	uint64 length = ip->size;
	uint64 va_start = BASE_ADDRESS;
	uint64 va_end = PGROUNDUP(BASE_ADDRESS + length);
	for (uint64 va = va_start, off = 0; va < va_end;
	     va += PGSIZE, off += PAGE_SIZE) {
		page = kalloc();
		if (page == 0) {
			panic("...");
		}
		readi(ip, 0, (uint64)page, off, PAGE_SIZE);
		if (off + PAGE_SIZE > length) {
			memset(page + (length - off), 0,
			       PAGE_SIZE - (length - off));
		}
		if (mappages(p->pagetable, va, PGSIZE, (uint64)page,
			     PTE_U | PTE_R | PTE_W | PTE_X) != 0)
			panic("...");
	}

	p->max_page = va_end / PAGE_SIZE;
	p->ustack_base = va_end + PAGE_SIZE;
	// alloc main thread
	if (allocthread(p, va_start, 1) != 0) {
		panic("proc %d alloc main thread failed!", p->pid);
	}
	debugf("bin loader fin");
	return 0;
}

// load all apps and init the corresponding `proc` structure.
int load_init_app()
{
	struct inode *ip;
	struct proc *p = allocproc();
	init_stdio(p);
	if ((ip = namei(INIT_PROC)) == 0) {
		errorf("invalid init proc name\n");
		return -1;
	}
	debugf("load init app %s", INIT_PROC);
	bin_loader(ip, p);
	iput(ip);
	char *argv[2];
	argv[0] = INIT_PROC;
	argv[1] = NULL;
	struct thread *t = &p->threads[0];
	t->trapframe->a0 = push_argv(p, argv);
	t->state = RUNNABLE;
	add_task(t);
	return 0;
}