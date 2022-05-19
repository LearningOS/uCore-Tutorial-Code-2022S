#ifndef VM_H
#define VM_H

#include "riscv.h"
#include "types.h"

void kvm_init();
void kvmmap(pagetable_t, uint64, uint64, uint64, int);
int mappages(pagetable_t, uint64, uint64, uint64, int);
pagetable_t uvmcreate();
int uvmcopy(pagetable_t, pagetable_t, uint64);
void uvmfree(pagetable_t, uint64);
int uvmmap(pagetable_t pagetable, uint64 va, uint64 npages, int perm);
void uvmunmap(pagetable_t, uint64, uint64, int);
uint64 walkaddr(pagetable_t, uint64);
uint64 useraddr(pagetable_t, uint64);
int copyout(pagetable_t, uint64, char *, uint64);
int copyin(pagetable_t, char *, uint64, uint64);
int copyinstr(pagetable_t, char *, uint64, uint64);
int either_copyout(int, uint64, char *, uint64);
int either_copyin(int, uint64, char *, uint64);

#endif // VM_H