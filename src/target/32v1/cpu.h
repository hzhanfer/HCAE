#ifndef CPU_H
#define CPU_H


#include <stdlib.h>
#include "api/cpu.h"


#define CPU_NUM_REGS 16
#define CPU_NUM_SREGS 3


typedef struct CPUCore {
    hcae_cpu_word
        regs[CPU_NUM_REGS],
        sregs[CPU_NUM_SREGS]
    ;
} CPUCore;


typedef struct CPU {
    HCAE_CPUModel* model;
    
    CPUCore* cores;
    size_t core_cnt;
} CPU;


struct HCAE_CPUModel {
    const char* name;
    
    size_t max_core_cnt;
};


CPU* create_cpu(void);
void destroy_cpu(CPU*);

int apply_cpu_settings(CPU*);
void set_cpu_model(CPU*, HCAE_CPUModel*);
void set_cpu_core_cnt(CPU*, size_t);


#endif