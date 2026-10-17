#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "cpu-run.h"
#include "core/machine.h"
#include "core/hware/memory.h"


static inline void step_cpu_core(CPUCore* core) {
    (void)core;
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
    
    hcae_cpu_word resetvec = // get addesss stored in reset vector
        *(hcae_cpu_word*)((uint8_t*)mem->block + HCAE_VECTORTABLE_OFF);
    core0->regs[HCAE_REG_PC] = resetvec; // set PC to address
    set_pst_act(core0, 1); // set PST.ACT = 1
    
    for (; mach->running;) step_cpu(cpu); // infinite loop
}