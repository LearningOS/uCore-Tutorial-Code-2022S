#ifndef DEFS_H
#define DEFS_H

#include "log.h"
#include "printf.h"
#include "riscv.h"
#include "sbi.h"
#include "types.h"

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

#endif // DEF_H