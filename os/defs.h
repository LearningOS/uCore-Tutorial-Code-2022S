#ifndef DEFS_H
#define DEFS_H

#include "const.h"
#include "log.h"
#include "printf.h"
#include "riscv.h"
#include "sbi.h"
#include "string.h"
#include "types.h"

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#endif // DEF_H
