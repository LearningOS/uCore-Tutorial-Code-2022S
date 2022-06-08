#ifndef SYSCALL_H
#define SYSCALL_H
#include "types.h"
#include "timer.h"

uint64 sys_gettimeofday(TimeVal *val, int _tz);
void syscall();

#endif // SYSCALL_H
