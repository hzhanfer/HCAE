#ifndef MACHINE_H
#define MACHINE_H


#include <stdbool.h>
#define HCAE_TARGET_CPU_HEADER "api/cpu.h"
#include "hcae/machine.h"
#include "cpu.h"
#include "core/hware/memory.h"


struct HCAE_Machine {
    bool
        running,
        settings_applied
    ;
    
    CPU* cpu;
    Memory* mem;
};


#endif