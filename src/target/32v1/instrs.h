#ifndef INSTRS_H
#define INSTRS_H


#include "cpu-run.h"
#include "opcodes.h"


typedef void (*instrfunc)(CPUCore*, Instr*);

typedef instrfunc instrtable[_OPCODE_ENDOFLIST][_OPERORDER_ENDOFLIST];


extern instrtable instrs;


#endif
