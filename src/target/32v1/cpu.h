#ifndef CPU_H
#define CPU_H


#include <stdlib.h>
#include "api/cpu.h"


#define CPU_NUM_REGS 16


typedef struct CPUCore {
    hcae_cpu_word regs[CPU_NUM_REGS];
} CPUCore;


typedef struct CPU {
    HCAE_CPUDefinition* def;
    
    CPUCore* cores;
    size_t core_cnt;
} CPU;


struct HCAE_CPUDefinition {
    const char* name;
    
    size_t max_core_cnt;
};


CPU* create_cpu(void);
void destroy_cpu(CPU*);

int apply_cpu_settings(CPU*);
void set_cpu_def(CPU*, HCAE_CPUDefinition*);
void set_cpu_core_cnt(CPU*, size_t);


#endif