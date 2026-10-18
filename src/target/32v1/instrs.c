#include <stdio.h>
#include "instrs.h"
#include "opcodes.h"
#include "cpu.h"
#include "cpu-run.h"


static void nop_nn(CPUCore* core, Instr* instr) {
    (void)core;
    (void)instr;
    
    printf("nop\n");
}

static void jmp_vn(CPUCore* core, Instr* instr) {
    core->regs[HCAE_REG_PC] = instr->val;
    
    printf("jmp [%x]\n", instr->val);
}


instrtable instrs = {
    [OPCODE_NOP] = {
        [OPERORDER_NN] = nop_nn
    },
    [OPCODE_LD] = {},
    [OPCODE_ST] = {},
    [OPCODE_JMP] = {
        [OPERORDER_VN] = jmp_vn
    }
};
