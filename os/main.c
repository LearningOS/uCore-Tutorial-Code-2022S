#include "console.h"
#include "defs.h"
#include "loader.h"
#include "trap.h"
uint64 __attribute__((noinline)) adddd(uint64 x, uint64 y) {
	return x+y;
}
int threadid()
{
	return 0;
}

void clean_bss()
{
	extern char s_bss[];
	extern char e_bss[];
	memset(s_bss, 0, e_bss - s_bss);
}

void main()
{
	uint64 r = adddd(10,100);
	printf("%d", r);
	clean_bss();
	printf("hello wrold!\n");
	printf("init trap\n");
	trap_init();
	loader_init();
	run_next_app();
}

