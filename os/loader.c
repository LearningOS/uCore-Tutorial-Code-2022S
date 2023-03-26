#include "loader.h"
#include "defs.h"
#include "trap.h"

static int app_cur, app_num;
static uint64 *app_info_ptr;
extern char _app_num[], userret[], boot_stack_top[], ekernel[];
/**
 * link_app.S汇编和此处结合加载应用程序
 * .quad 设置一个64位的值
 * 这里表示 _app_num可视为 *uint64类型变量的值,该指针第一个值为应用的数量
 * 接着依次是第1，2，3个应用的地址.
 * .incbin用于整个应用的可执行二进制文件加载
 */
void loader_init()
{
	if ((uint64)ekernel >= BASE_ADDRESS) {
		panic("kernel too large...\n");
	}
	// _app_num是link_app.S中定义的全局符号，是一个保存地址的指针
	app_info_ptr = (uint64 *)_app_num;
	app_cur = -1;
	// app_num表示应用的数量
	app_num = *app_info_ptr;

}
/**
 * user_stack为用户的栈，这里的话堆栈会在.data处
 */
__attribute__((aligned(4096))) char user_stack[USER_STACK_SIZE];
__attribute__((aligned(4096))) char trap_page[TRAP_PAGE_SIZE];

int load_app(uint64 *info)
{
	/**
	 * 将应用加载到[BASE_ADDRESS, BASE_ADDRESS + MAX_APP_SIZE]处
	 */
	uint64 start = info[0], end = info[1], length = end - start;
	memset((void *)BASE_ADDRESS, 0, MAX_APP_SIZE);
	memmove((void *)BASE_ADDRESS, (void *)start, length);
	return length;
}

int run_next_app()
{
	// trap_page
	struct trapframe *trapframe = (struct trapframe *)trap_page;
	app_cur++;
	//app_info_ptr[0] [1] ,[2]. [3]，分别保存着应用的数量，第1，2，3个应用的地址
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