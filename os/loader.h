#ifndef LOADER_H
#define LOADER_H

#include "const.h"
#include "types.h"

int finished();
void loader_init();
int run_all_app();

#define BASE_ADDRESS (0x1000)
#define USTACK_SIZE (PAGE_SIZE)
#define KSTACK_SIZE (PAGE_SIZE)
#define TRAP_PAGE_SIZE (PAGE_SIZE)

#endif // LOADER_H