#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "cpu-run.h"
#include "opcodes.h"
#include "instrs.h"
#include "core/machine.h"
#include "core/hware/memory.h"


void cpu_core_syncexcept(CPUCore* core, SyncExceptType type) {
    Memory* mem = core->mem;
    
    core->sregs[HCAE_SREG_ELR] = core->regs[HCAE_REG_PC]; // set ELR
    core->sregs[HCAE_SREG_EDAT] = type;
    memcpy((uint8_t*)&core->regs[HCAE_REG_PC],
           (uint8_t*)mem->block + HCAE_VECTORTABLE_SYNCEXCEPT_ADDR,
           sizeof(core->regs[HCAE_REG_PC]));
    
    printf("// Synchronous Exception #%x at %08x\n", core->sregs[HCAE_SREG_EDAT], core->sregs[HCAE_SREG_ELR]);
}


static inline int fetch_cpu_core_instr(CPUCore* core, Instr* instr, hcae_cpu_word* out_instr_addr) {
    Memory* mem = core->mem;
    
    hcae_cpu_word instr_addr = core->regs[HCAE_REG_PC]; // start of the instruction
    *out_instr_addr = instr_addr;
    
    if (!mem_read(mem, core->regs[HCAE_REG_PC], (uint8_t*)&instr->opcode)) { // lower 8 opcode bits
        core->regs[HCAE_REG_PC] = instr_addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return 0;
    }
    core->regs[HCAE_REG_PC]++;
    
    uint8_t second_byte;
    if (!mem_read(mem, core->regs[HCAE_REG_PC], &second_byte)) {
        core->regs[HCAE_REG_PC] = instr_addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return 0;
    }
    core->regs[HCAE_REG_PC]++;
    
    instr->opcode |= ((uint16_t)(second_byte & 0x07)) << 8; // decode opcode
    if (instr->opcode <= _OPCODE_UDF || instr->opcode >= _OPCODE_ENDOFLIST) { // unknown opcode
        core->regs[HCAE_REG_PC] = instr_addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_UI);
        return 0;
    }
    
    uint8_t oper_ord = (second_byte >> 3) & 0x7; // decode operand order
    if (oper_ord >= _OPERORDER_ENDOFLIST) { // unknown operand order
        core->regs[HCAE_REG_PC] = instr_addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_UI);
        return 0;
    }
    
    if (oper_ord == OPERORDER_RN || // fetch and decode register operands
        oper_ord == OPERORDER_RR ||
        oper_ord == OPERORDER_RV)
    {
        uint8_t regs_byte;
        
        if (!mem_read(mem, core->regs[HCAE_REG_PC], &regs_byte)) {
            core->regs[HCAE_REG_PC] = instr_addr;
            cpu_core_syncexcept(core, SYNCEXCEPT_MF);
            return 0;
        }
        core->regs[HCAE_REG_PC]++;
        
        instr->rd = regs_byte & 0xf; // lower 4-bits
        instr->rs = (regs_byte >> 4) & 0xf; // upper 4-bits
    }
    
    if (oper_ord == OPERORDER_RV || // fetch value
        oper_ord == OPERORDER_VN)
    {
        instr->val_len = ((second_byte >> 6) & 0x3) + 1;
        
        if (!mem_read_arr(mem, core->regs[HCAE_REG_PC], (uint8_t*)&instr->val, instr->val_len)) {
            core->regs[HCAE_REG_PC] = instr_addr;
            cpu_core_syncexcept(core, SYNCEXCEPT_MF);
            return 0;
        }
        core->regs[HCAE_REG_PC] += instr->val_len;
    }
    
    instr->oper_ord = oper_ord;
    return 1;
}

static inline void step_cpu_core(CPUCore* core) {
    Instr instr = {0};
    
    hcae_cpu_word instr_addr;
    
    if (!fetch_cpu_core_instr(core, &instr, &instr_addr))
        return;
    
    instrfunc ifun = instrs[instr.opcode][instr.oper_ord];
    if (!ifun) { // unknown instruction
        core->regs[HCAE_REG_PC] = instr_addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_UI);
        return;
    }
    
    ifun(core, &instr);
}

static inline void step_cpu(CPU* cpu) {
    for (size_t i = 0; i < cpu->core_cnt; i++) {
        CPUCore* core = &cpu->cores[i];
        
        if (get_pst_act(core) != 1) continue; // not active
        
        step_cpu_core(core);
    }
}

void run_cpu(CPU* cpu) {
    CPUCore* core0 = &cpu->cores[0];
    HCAE_Machine* mach = cpu->mach;
    Memory* mem = mach->mem;
    
    memcpy((uint8_t*)&core0->regs[HCAE_REG_PC],
           (uint8_t*)mem->block + HCAE_VECTORTABLE_RESET_ADDR,
           sizeof(core0->regs[HCAE_REG_PC]));
    set_pst_act(core0, 1); // set PST.ACT = 1
    
    for (; mach->running;) step_cpu(cpu); // infinite loop
}