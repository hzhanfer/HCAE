#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "cpu-run.h"
#include "cpu-api/cpu.h"
#include "cpu-api/opcodes.h"
#include "instrs.h"
#include "core/machine.h"
#include "core/hware/memory.h"


void cpu_core_syncexcept(CPUCore* core, HCAE_32V1SyncException type) {
    Memory* mem = core->mem;
    
    core->sregs[HCAE_32V1_SR_ILR] = core->regs[HCAE_32V1_SPR_PC]; // set ILR
    core->sregs[HCAE_32V1_SR_IDAT] = type;
    memcpy((uint8_t*)&core->regs[HCAE_32V1_SPR_PC],
           (uint8_t*)mem->block + HCAE_32V1_VECTTAB_SYNCEXCEPT_ADDR,
           sizeof(core->regs[HCAE_32V1_SPR_PC]));
}


static inline int fetch_cpu_core_instr(CPUCore* core, Instr* instr) {
    Memory* mem = core->mem;
    
    instr->addr = core->regs[HCAE_32V1_SPR_PC]; // start of the instruction
    
    if (!mem_read(mem, core->regs[HCAE_32V1_SPR_PC], (uint8_t*)&instr->opcode)) { // lower 8 opcode bits
        core->regs[HCAE_32V1_SPR_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF);
        return 0;
    }
    core->regs[HCAE_32V1_SPR_PC]++;
    
    uint8_t second_byte;
    if (!mem_read(mem, core->regs[HCAE_32V1_SPR_PC], &second_byte)) {
        core->regs[HCAE_32V1_SPR_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF);
        return 0;
    }
    core->regs[HCAE_32V1_SPR_PC]++;
    
    instr->opcode |= (HCAE_32V1Opcode)(((uint16_t)(second_byte & 0x07)) << 8); // decode opcode
    if (instr->opcode <= _HCAE_32V1_OPCODE_STARTOFLIST || instr->opcode >= _HCAE_32V1_OPCODE_ENDOFLIST) { // unknown opcode
        core->regs[HCAE_32V1_SPR_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_UI);
        return 0;
    }
    
    uint8_t oper_ord = (second_byte >> 3) & 0x7; // decode operand order
    if (oper_ord >= _HCAE_32V1_OPERORDER_ENDOFLIST) { // unknown operand order
        core->regs[HCAE_32V1_SPR_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_UI);
        return 0;
    }
    
    if (oper_ord == HCAE_32V1_OPERORDER_RN || // fetch and decode register operands
        oper_ord == HCAE_32V1_OPERORDER_RR ||
        oper_ord == HCAE_32V1_OPERORDER_RV)
    {
        uint8_t regs_byte;
        
        if (!mem_read(mem, core->regs[HCAE_32V1_SPR_PC], &regs_byte)) {
            core->regs[HCAE_32V1_SPR_PC] = instr->addr;
            cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF);
            return 0;
        }
        core->regs[HCAE_32V1_SPR_PC]++;
        
        instr->rd = (HCAE_32V1RegisterIndex)(regs_byte & 0xf); // lower 4-bits
        instr->rs = (HCAE_32V1RegisterIndex)((regs_byte >> 4) & 0xf); // upper 4-bits
    }
    
    instr->val_len = (((second_byte >> 6) & 0x3) + 1;
        
    if (oper_ord == HCAE_32V1_OPERORDER_RV || // fetch value
        oper_ord == HCAE_32V1_OPERORDER_VN)
    {
        if (!mem_read_arr(mem, core->regs[HCAE_32V1_SPR_PC], (uint8_t*)&instr->val, instr->val_len)) {
            core->regs[HCAE_32V1_SPR_PC] = instr->addr;
            cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF);
            return 0;
        }
        core->regs[HCAE_32V1_SPR_PC] += instr->val_len;
    }
    
    instr->oper_ord = (HCAE_32V1OperandOrder)oper_ord;
    return 1;
}

static inline void step_cpu_core(CPUCore* core) {
    Instr instr = {0};
    
    if (!fetch_cpu_core_instr(core, &instr))
        return;
    
    instrfunc ifun = instrs[instr.opcode][instr.oper_ord];
    if (!ifun) { // unknown instruction
        core->regs[HCAE_32V1_SPR_PC] = instr.addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_UI);
        return;
    }
    
    printf("%08x\t", instr.addr);
    ifun(core, &instr);
}

static inline void step_cpu(CPU* cpu) {
    for (size_t i = 0; i < cpu->core_cnt; i++) {
        CPUCore* core = &cpu->cores[i];
        
        if ((core->sregs[HCAE_32V1_SR_PST] & HCAE_32V1_PST_ACT_MASK) != 1)
            continue; // not active
        
        step_cpu_core(core);
    }
}

void run_cpu(CPU* cpu, size_t step_cnt) {
    CPUCore* core0 = &cpu->cores[0];
    HCAE_Machine* mach = cpu->mach;
    Memory* mem = mach->mem;
    
    memcpy((uint8_t*)&core0->regs[HCAE_32V1_SPR_PC],
           (uint8_t*)mem->block + HCAE_32V1_VECTTAB_RESET_ADDR,
           sizeof(core0->regs[HCAE_32V1_SPR_PC]));
    
    core0->sregs[HCAE_32V1_SR_PST] = (core0->sregs[HCAE_32V1_SR_PST] & ~HCAE_32V1_PST_ACT_MASK) | (1 << HCAE_32V1_PST_ACT_SHIFT);
    
    if (step_cnt == (size_t)HCAE_RUNINFINITE) {
        for (; mach->running;)
            step_cpu(cpu);
    } else {
        for (size_t i = 0; mach->running && i < step_cnt; i++)
            step_cpu(cpu); // infinite loop
        
        mach->running = false;
    }
}