#ifndef CPU_H
#define CPU_H


#include <stdlib.h>
#include "api/cpu.h"


#define CPU_NUM_REGS 16
#define CPU_NUM_SREGS 3


typedef struct HCAE_Machine HCAE_Machine;

typedef struct Memory Memory;

typedef struct CPU CPU;


typedef struct CPUCore {
    CPU* cpu;
    Memory* mem;
    
    HCAE_CPUWord
        regs[CPU_NUM_REGS], // general
        sregs[CPU_NUM_SREGS] // system
    ;
} CPUCore;


struct CPU {
    HCAE_Machine* mach;
    HCAE_CPUModel* model;
    
    CPUCore* cores;
    size_t core_cnt;
};


struct HCAE_CPUModel {
    const char* name;
    
    size_t max_core_cnt;
};


CPU* create_cpu(HCAE_Machine*);
void destroy_cpu(CPU*);

int apply_cpu_settings(CPU*);
void set_cpu_model(CPU*, HCAE_CPUModel*);
void set_cpu_core_cnt(CPU*, size_t);

void run_cpu(CPU*, size_t);


#endif