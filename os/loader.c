#include "loader.h"
#include "defs.h"
#include "trap.h"

static int app_cur, app_num;
static uint64 *app_info_ptr;
extern char _app_num[], userret[], boot_stack_top[], ekernel[];

void loader_init()
{
	if ((uint64)ekernel >= BASE_ADDRESS) {
		panic("kernel too large...\n");
	}
	app_info_ptr = (uint64 *)_app_num;
	app_cur = -1;
	app_num = *app_info_ptr;
}

__attribute__((aligned(4096))) char user_stack[USER_STACK_SIZE];
__attribute__((aligned(4096))) char trap_page[TRAP_PAGE_SIZE];

int load_app(uint64 *info)
{
	uint64 start = info[0], end = info[1], length = end - start;
	memset((void *)BASE_ADDRESS, 0, MAX_APP_SIZE);
	memmove((void *)BASE_ADDRESS, (void *)start, length);
	return length;
}

int run_next_app()
{
	struct trapframe *trapframe = (struct trapframe *)trap_page;
	app_cur++;
	app_info_ptr++;
	if (app_cur >= app_num) {
		return -1;
	}
	infof("load and run app %d", app_cur);
	uint64 length = load_app(app_info_ptr);
	debugf("bin range = [%p, %p)", *app_info_ptr, *app_info_ptr + length);
	memset(trapframe, 0, 4096);
	trapframe->epc = BASE_ADDRESS;
	trapframe->sp = (uint64)user_stack + USER_STACK_SIZE;
	usertrapret(trapframe, (uint64)boot_stack_top);
	return 0;
}