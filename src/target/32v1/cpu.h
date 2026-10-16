#ifndef CPU_H
#define CPU_H


#include <stdlib.h>
#include "api/cpu.h"


#define CPU_NUM_REGS 16


typedef struct CPUCore {
    hcae_cpu_word regs[CPU_NUM_REGS];
} CPUCore;


typedef struct CPU {
    CPUCore* cores;
    size_t core_cnt;
} CPU;


#endif