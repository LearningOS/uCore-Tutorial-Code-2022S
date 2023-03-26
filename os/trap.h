#ifndef TRAP_H
#define TRAP_H

#include "types.h"

/**
 * 该结构用于保存用户陷入内核后的用户上下文和一些内核信息，位于trap_page[TRAP_PAGE_SIZE]中
 */
struct trapframe {
	/*   0 */ uint64 kernel_satp; // kernel page table， 是否用户态上下文？
	/*   8 */ uint64 kernel_sp; // top of process's kernel stack  非用户态上下文
	/*  16 */ uint64 kernel_trap; // usertrap() 是否用户态上下文？
	/*  24 */ uint64 epc; // saved user program counter
	/*  32 */ uint64 kernel_hartid; // saved kernel tp 是否用户态上下文？
	/*  40 */ uint64 ra;
	/*  48 */ uint64 sp;
	/*  56 */ uint64 gp;
	/*  64 */ uint64 tp;
	/*  72 */ uint64 t0;
	/*  80 */ uint64 t1;
	/*  88 */ uint64 t2;
	/*  96 */ uint64 s0;
	/* 104 */ uint64 s1;
	/* 112 */ uint64 a0;
	/* 120 */ uint64 a1;
	/* 128 */ uint64 a2;
	/* 136 */ uint64 a3;
	/* 144 */ uint64 a4;
	/* 152 */ uint64 a5;
	/* 160 */ uint64 a6;
	/* 168 */ uint64 a7;
	/* 176 */ uint64 s2;
	/* 184 */ uint64 s3;
	/* 192 */ uint64 s4;
	/* 200 */ uint64 s5;
	/* 208 */ uint64 s6;
	/* 216 */ uint64 s7;
	/* 224 */ uint64 s8;
	/* 232 */ uint64 s9;
	/* 240 */ uint64 s10;
	/* 248 */ uint64 s11;
	/* 256 */ uint64 t3;
	/* 264 */ uint64 t4;
	/* 272 */ uint64 t5;
	/* 280 */ uint64 t6;
};

enum Exception {
	InstructionMisaligned = 0,
	InstructionAccessFault = 1,
	IllegalInstruction = 2,
	Breakpoint = 3,
	LoadMisaligned = 4,
	LoadAccessFault = 5,
	StoreMisaligned = 6,
	StoreAccessFault = 7,
	UserEnvCall = 8,
	SupervisorEnvCall = 9,
	MachineEnvCall = 11,
	InstructionPageFault = 12,
	LoadPageFault = 13,
	StorePageFault = 15,
};

enum Interrupt {
	UserSoft = 0,
	SupervisorSoft,
	UserTimer = 4,
	SupervisorTimer,
	UserExternal = 8,
	SupervisorExternal,
};

void trap_init();
void usertrapret(struct trapframe *trapframe, uint64 kstack);

#endif // TRAP_H