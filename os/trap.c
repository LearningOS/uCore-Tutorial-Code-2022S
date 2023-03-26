#include "trap.h"
#include "defs.h"
#include "loader.h"
#include "syscall.h"

extern char trampoline[], uservec[], boot_stack_top[];
extern void *userret(uint64);

// set up to take exceptions and traps while in the kernel.
/**
 * 根据riscv的调用规范，位于u模式的用户程序，需要通过ecall指令来陷入s模式，进而获取运行在s模式的内核提供的服务。
 * 这种调用规范，也称为系统调用，他需要一系列的寄存器的配合，而这超出了高级编程语言的表达能力，因此是用汇编语言编写的。
 * 当处于u用户态的程序触发ecall指令陷入s模式时，会发生一系列变化，scause和stval保存异常的原因和附加信息，
 * sepc保存着异常发生之前执行的最后一条指令，在ecall中，指的就是ecall指令的地址，并且返回时sepc要+4 以便执行ecall后的指令。
 * ，接着cpu跳转到stvec所保存的函数起始地址，开始处理trap。通常的用法是在调用ecall之前，将参数保存在a0，a1，a2，...寄存器中，将系统调用序号
 * 保存在a7中，并且将返回值保存在a0中。
 *
 * trap_init，将处理trap的函数地址设置为uservec，该函数在trampoline.S中定义。
 */
void trap_init(void)
{
	w_stvec((uint64)uservec & ~0x3);
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void usertrap(struct trapframe *trapframe)
{
	printf("trap into kernel\n");
	if ((r_sstatus() & SSTATUS_SPP) != 0)
		panic("usertrap: not from user mode");

	uint64 cause = r_scause();
	// ecall 调用 的cause 为8
	if (cause == UserEnvCall) {
		// epc+4，使其指向ecall之后的地址
		trapframe->epc += 4;
		syscall(trapframe);
		return usertrapret(trapframe, (uint64)boot_stack_top);
	}
	switch (cause) {
	case StoreMisaligned:
	case StorePageFault:
	case LoadMisaligned:
	case LoadPageFault:
	case InstructionMisaligned:
	case InstructionPageFault:
		errorf("%d in application, bad addr = %p, bad instruction = %p, core " "dumped.",
		       cause, r_stval(), trapframe->epc);
		break;
	case IllegalInstruction:
		printf("IllegalInstruction in application, epc = %p, core dumped.",
		       trapframe->epc);
		break;
	default:
		errorf("unknown trap: %p, stval = %p sepc = %p", r_scause(),
		       r_stval(), r_sepc());
		break;
	}
	//如果是异常，则结束该程序，开始下一个程序
	infof("switch to next app");
	run_next_app();
	printf("ALL DONE\n");
	shutdown();
}

//
// return to user space
//
void usertrapret(struct trapframe *trapframe, uint64 kstack)
{
	printf("return to user space\n");
	trapframe->kernel_satp = r_satp(); // kernel page table
	trapframe->kernel_sp = kstack + PGSIZE; // process's kernel stack
	trapframe->kernel_trap = (uint64)usertrap;
	trapframe->kernel_hartid = r_tp(); // hartid for cpuid()
	// sret返回用户态，并跳转到sepc所指处继续执行，这是是用户程序的地址。
	w_sepc(trapframe->epc);
	// set up the registers that trampoline.S's sret will use
	// to get to user space.

	// set S Previous Privilege mode to User.
	uint64 x = r_sstatus();
	// 因为我们接下来要返回用户态，sret指令会干两件事，第一是按照sstatus中SPP字段设置特权级，所以这里SPP设置为0，第二是跳转到sepc指向的指向指令
	x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
	x |= SSTATUS_SPIE; // enable interrupts in user mode
	w_sstatus(x);

	// tell trampoline.S the user page table to switch to.
	// uint64 satp = MAKE_SATP(p->pagetable);
	userret((uint64)trapframe);
	// 内核加载完第一个应用之后，执行userret之前，还是处于启动状态，此时的栈还是boot_stack_top,不过已经准备了一些，比如设置好了sepc和sstatus
	//trapframe中kernel_stap，kernel_sp,kernel_trap，epc，sret之后，用户完全恢复了上下文，主要包括栈指针，epc程序计数器，和其他普通寄存器
	// 并且sscratch指向用户程序的trapframe结构
}