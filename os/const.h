#ifndef CONST_H
#define CONST_H

#define PAGE_SIZE (0x1000)

enum {
	STDIN = 0,
	STDOUT = 1,
	STDERR = 2,
};

// memory layout

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to PHYSTOP.
#define KERNBASE 0x80200000L
#define PHYSTOP (0x80000000 + 128 * 1024 * 1024) // we have 128M memroy

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))

// map the trampoline page to the highest address,
// in both user and kernel space.
#define USER_TOP (MAXVA)
#define TRAMPOLINE (USER_TOP - PGSIZE)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)

// memory layout end

#define MAX_STR_LEN (200)

#endif // CONST_H