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
    
    core->sregs[HCAE_SREG_ILR] = core->regs[HCAE_REG_PC]; // set ELR
    core->sregs[HCAE_SREG_IDAT] = type;
    memcpy((uint8_t*)&core->regs[HCAE_REG_PC],
           (uint8_t*)mem->block + HCAE_VECTORTABLE_SYNCEXCEPT_ADDR,
           sizeof(core->regs[HCAE_REG_PC]));
    
    printf("// Synchronous Exception (type: %x) at 0x%08x\n", core->sregs[HCAE_SREG_EDAT], core->sregs[HCAE_SREG_ELR]);
}


static inline int fetch_cpu_core_instr(CPUCore* core, Instr* instr) {
    Memory* mem = core->mem;
    
    instr->addr = core->regs[HCAE_REG_PC]; // start of the instruction
    
    if (!mem_read(mem, core->regs[HCAE_REG_PC], (uint8_t*)&instr->opcode)) { // lower 8 opcode bits
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return 0;
    }
    core->regs[HCAE_REG_PC]++;
    
    uint8_t second_byte;
    if (!mem_read(mem, core->regs[HCAE_REG_PC], &second_byte)) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return 0;
    }
    core->regs[HCAE_REG_PC]++;
    
    instr->opcode |= ((uint16_t)(second_byte & 0x07)) << 8; // decode opcode
    if (instr->opcode <= _OPCODE_UDF || instr->opcode >= _OPCODE_ENDOFLIST) { // unknown opcode
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_UI);
        return 0;
    }
    
    uint8_t oper_ord = (second_byte >> 3) & 0x7; // decode operand order
    if (oper_ord >= _OPERORDER_ENDOFLIST) { // unknown operand order
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_UI);
        return 0;
    }
    
    if (oper_ord == OPERORDER_RN || // fetch and decode register operands
        oper_ord == OPERORDER_RR ||
        oper_ord == OPERORDER_RV)
    {
        uint8_t regs_byte;
        
        if (!mem_read(mem, core->regs[HCAE_REG_PC], &regs_byte)) {
            core->regs[HCAE_REG_PC] = instr->addr;
            cpu_core_syncexcept(core, SYNCEXCEPT_MF);
            return 0;
        }
        core->regs[HCAE_REG_PC]++;
        
        instr->rd = regs_byte & 0xf; // lower 4-bits
        instr->rs = (regs_byte >> 4) & 0xf; // upper 4-bits
    }
    
    instr->val_len = ((second_byte >> 6) & 0x3) + 1;
        
    if (oper_ord == OPERORDER_RV || // fetch value
        oper_ord == OPERORDER_VN)
    {
        if (!mem_read_arr(mem, core->regs[HCAE_REG_PC], (uint8_t*)&instr->val, instr->val_len)) {
            core->regs[HCAE_REG_PC] = instr->addr;
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
    
    if (!fetch_cpu_core_instr(core, &instr))
        return;
    
    instrfunc ifun = instrs[instr.opcode][instr.oper_ord];
    if (!ifun) { // unknown instruction
        core->regs[HCAE_REG_PC] = instr.addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_UI);
        return;
    }
    
    printf("%08x\t", instr.addr);
    ifun(core, &instr);
}

static inline void step_cpu(CPU* cpu) {
    for (size_t i = 0; i < cpu->core_cnt; i++) {
        CPUCore* core = &cpu->cores[i];
        
        if ((core->sregs[HCAE_SREG_PST] & PST_ACT_MASK) != 1)
            continue; // not active
        
        step_cpu_core(core);
    }
}

void run_cpu(CPU* cpu, size_t step_cnt) {
    CPUCore* core0 = &cpu->cores[0];
    HCAE_Machine* mach = cpu->mach;
    Memory* mem = mach->mem;
    
    memcpy((uint8_t*)&core0->regs[HCAE_REG_PC],
           (uint8_t*)mem->block + HCAE_VECTORTABLE_RESET_ADDR,
           sizeof(core0->regs[HCAE_REG_PC]));
    
    core0->sregs[HCAE_SREG_PST] = (core0->sregs[HCAE_SREG_PST] & ~PST_ACT_MASK) | (1 << PST_ACT_SHIFT);
    
    if (step_cnt == HCAE_MACHINE_RUNINFINITE) {
        for (; mach->running;)
            step_cpu(cpu);
    } else {
        for (size_t i = 0; mach->running && i < step_cnt; i++)
            step_cpu(cpu); // infinite loop
        
        mach->running = false;
    }
}