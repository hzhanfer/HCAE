#ifndef CPU_H
#define CPU_H


#include <stdlib.h>
#include "cpu-api/cpu.h"


typedef struct HCAE_Machine HCAE_Machine;

typedef struct Memory Memory;

typedef struct CPU CPU;


typedef struct CPUCore {
    CPU* cpu;
    Memory* mem;
    
    HCAE_32V1Word
        regs[_HCAE_32V1_RR_ENDOFLIST], // general
        sregs[_HCAE_32V1_SR_ENDOFLIST] // system
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