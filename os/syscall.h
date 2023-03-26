#ifndef SYSCALL_H
#define SYSCALL_H
#include <trap.h>
void syscall(struct trapframe * trapF);

#endif // SYSCALL_H
