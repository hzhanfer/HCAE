#ifndef CPU_H
#define CPU_H


#include <stdlib.h>

#include "hcae/cpu.h"
#include "arch.h"


#define CPUMODEL_MAX_DEP_CNT ((size_t)-1)


typedef struct HCAE_Board HCAE_Board;


struct HCAE_CPUModel {
    size_t cpu_dep_cnt; // number of CPUs that depend on this model

    const char* name;

    size_t
        default_core_cnt,
        max_core_cnt
    ;

    HCAE_CPUArchModel archmodel;
};


struct HCAE_CPU {
    HCAE_Board* brd;

    HCAE_CPUModel* model;

    HCAE_CPUArch* cores; // cores
    size_t core_cnt;
};


#endif