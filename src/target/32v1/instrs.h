#ifndef INSTRS_H
#define INSTRS_H


#include "cpu-run.h"
#include "cpu-api/opcodes.h"


typedef void (*instrfunc)(CPUCore*, Instr*);

typedef instrfunc instrtable[_HCAE_32V1_OPCODE_ENDOFLIST][_HCAE_32V1_OPERORDER_ENDOFLIST];


extern instrtable instrs;


#endif
