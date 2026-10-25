#ifndef ARCH_H
#define ARCH_H


#include "cpu-api/arch.h"


typedef struct HCAE_Board HCAE_Board;

typedef struct HCAE_CPU HCAE_CPU;
typedef struct HCAE_CPUModel HCAE_CPUModel;


struct HCAE_32V1 {
    HCAE_CPU* cpu;
    HCAE_Board* brd;
    HCAE_CPUModel* model;

    HCAE_CPUWord
        regs[_HCAE_32V1_REG_ENDOFLIST],
        sregs[_HCAE_32V1_SREG_ENDOFLIST]
    ;
};


struct HCAE_32V1Model {
    int dummy; // nothing here yet...
};


int init_32v1(HCAE_32V1*, HCAE_CPU*, HCAE_Board*, HCAE_CPUModel*);
void uninit_32v1(HCAE_32V1*);

int init_32v1_model(HCAE_32V1Model*, HCAE_CPUModel*);
void uninit_32v1_model(HCAE_32V1Model*);


#define init_cpu_arch(_core, _cpu, _brd, _model) init_32v1((_core), (_cpu), (_brd), (_model))
#define uninit_cpu_arch(_core) uninit_32v1((_core))

#define init_cpu_arch_model(_model, _cpu_model) init_32v1_model((_model), (_cpu_model))
#define uninit_cpu_arch_model(_model) uninit_32v1_model((_model))


#endif