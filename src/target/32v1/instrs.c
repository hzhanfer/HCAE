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


static void ld_rr(CPUCore* core, Instr* instr) {
    printf("ld.%d\tr%d, [r%d]\n", instr->val_len, instr->rd, instr->rs);
    
    hcae_cpu_word w = 0;
    
    if (!mem_read_arr(core->mem,
                      core->regs[instr->rs],
                      (uint8_t*)&w,
                      instr->val_len)
    ) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return;
    }
    
    core->regs[instr->rd] = w;
}


static void st_rr(CPUCore* core, Instr* instr) {
    printf("st.%d\t[r%d], r%d\n", instr->val_len, instr->rd, instr->rs);

    if (!mem_write_arr(core->mem,
                       core->regs[instr->rd],
                       (uint8_t*)&core->regs[instr->rs],
                       instr->val_len)
    ) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return;
    }
}

static void st_rv(CPUCore* core, Instr* instr) {
    printf("st.%d\t[r%d], #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    if (!mem_write_arr(core->mem,
                       core->regs[instr->rd],
                       (uint8_t*)&instr->val,
                       instr->val_len)
    ) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return;
    }
}


static void jmp_rn(CPUCore* core, Instr* instr) {
    printf("jmp\tr%d\n", instr->rs);
    
    core->regs[HCAE_REG_PC] = core->regs[instr->rs];
}

static void jmp_vn(CPUCore* core, Instr* instr) {
    printf("jmp\t#0x%x\n", instr->val);
    
    core->regs[HCAE_REG_PC] += instr->val; // PC relative jump
}


static void put_rr(CPUCore* core, Instr* instr) {
    printf("put\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] = core->regs[instr->rs];
}

static void put_rv(CPUCore* core, Instr* instr) {
    printf("put\tr%d, #0x%x\n", instr->rd, instr->val);
    
    core->regs[instr->rd] = instr->val;
}


instrtable instrs = {
    [OPCODE_NOP] = {[OPERORDER_NN] = nop_nn},
    [OPCODE_LD] = {[OPERORDER_RR] = ld_rr},
    [OPCODE_ST] = {[OPERORDER_RR] = st_rr, [OPERORDER_RV] = st_rv},
    [OPCODE_JMP] = {[OPERORDER_RN] = jmp_rn, [OPERORDER_VN] = jmp_vn},
    [OPCODE_PUT] = {[OPERORDER_RR] = put_rr, [OPERORDER_RV] = put_rv}
};
