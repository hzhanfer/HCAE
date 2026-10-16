#ifndef MACHINE_H
#define MACHINE_H


#include "hcae/machine.h"
#include "cpu.h"


struct HCAE_Machine {
    CPU* cpu;
};


#endif