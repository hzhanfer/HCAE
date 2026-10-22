#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "core/machine.h"
#include "cpu.h"
#include "core/hware/memory.h"
#include "core/error.h"


HCAE_Machine* HCAE_CreateMachine(void) {
    if (HCAE_GetError() != HCAE_ERROR_N) return 0;
    
    HCAE_Machine* mach = calloc(1, sizeof(HCAE_Machine));
    if (!mach) {
        set_error(HCAE_ERROR_OOM);
        return NULL;
    }
    
    CPU* cpu = create_cpu(mach);
    if (!cpu) {
        free(mach);
        return NULL;
    }
    
    Memory* mem = create_memory();
    if (!mem) {
        destroy_cpu(cpu);
        free(mach);
        return NULL;
    }
    
    mach->cpu = cpu;
    mach->mem = mem;
    return mach;
}

void HCAE_DestroyMachine(HCAE_Machine* mach) {
    if (!mach) {
        set_error(HCAE_ERROR_II);
        return;
    }
    
    destroy_cpu(mach->cpu);
    destroy_memory(mach->mem);
    free(mach);
}


int HCAE_ApplyMachineSettings(HCAE_Machine* mach) {
    if (HCAE_GetError() != HCAE_ERROR_N) return 0;
    
    if (!mach) {
        set_error(HCAE_ERROR_II);
        return 0;
    }
    
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_SHABA);
        return 0;
    }
    
    if (!apply_cpu_settings(mach->cpu)) return 0;
    if (!apply_memory_settings(mach->mem)) return 0;
    
    mach->settings_applied = true;
    return 1;
}


void HCAE_SetMachineCPUModel(HCAE_Machine* mach, HCAE_CPUModel* model) {
    if (HCAE_GetError() != HCAE_ERROR_N) return;
    
    if (!mach || !model) {
        set_error(HCAE_ERROR_II);
        return;
    }
    
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    mach->cpu->model = model;
}

void HCAE_SetMachineCPUCoreCount(HCAE_Machine* mach, size_t cnt) {
    if (HCAE_GetError() != HCAE_ERROR_N) return;
    
    if (!mach) {
        set_error(HCAE_ERROR_II);
        return;
    }
    
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    set_cpu_core_cnt(mach->cpu, cnt);
}


void HCAE_SetMachineRAM(HCAE_Machine* mach, HCAE_CPUWord bot, HCAE_CPUWord size) {
    if (HCAE_GetError() != HCAE_ERROR_N) return;
    
    if (!mach) {
        set_error(HCAE_ERROR_II);
        return;
    }
    
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    set_ram(mach->mem, bot, size);
}

void HCAE_SetMachineROM(HCAE_Machine* mach, HCAE_CPUWord bot, HCAE_CPUWord size) {
    if (HCAE_GetError() != HCAE_ERROR_N) return;
    
    if (!mach) {
        set_error(HCAE_ERROR_II);
        return;
    }
    
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    set_rom(mach->mem, bot, size);
}

void HCAE_SetMachineMMIO(HCAE_Machine* mach, HCAE_CPUWord bot, HCAE_CPUWord size) {
    if (HCAE_GetError() != HCAE_ERROR_N) return;
    
    if (!mach) {
        set_error(HCAE_ERROR_II);
        return;
    }
    
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    set_mmio(mach->mem, bot, size);
}

int HCAE_CopyDataToMachineROM(
    HCAE_Machine* mach,
    const uint8_t* data,
    size_t size,
    HCAE_CPUWord pos) 
{
    if (HCAE_GetError() != HCAE_ERROR_N) return 0;
    
    if (!mach || !data) {
        set_error(HCAE_ERROR_II);
        return 0;
    }
    
    if (!mach->settings_applied) {
        set_error(HCAE_ERROR_ORSTBA);
        return 0;
    }
    
    if (pos < mach->mem->rom.bot || (size_t)pos + size > mach->mem->rom.top) {
        set_error(HCAE_ERROR_OOB);
        return 0;
    }
    
    memcpy(mach->mem->block + mach->mem->rom.bot + pos, data, size);
    return 1;
}


int HCAE_RunMachine(HCAE_Machine* mach, size_t step_cnt) {
    if (!mach) {
        set_error(HCAE_ERROR_II);
        return 0;
    }
    
    if (!mach->settings_applied) {
        set_error(HCAE_ERROR_ORSTBA);
        return 0;
    }
    
    if (mach->running) {
        set_error(HCAE_ERROR_MAR);
        return 0;
    }
    
    mach->running = true;
    
    run_cpu(mach->cpu, step_cnt);
    
    return 1;
}


int HCAE_GetMachineCPUCoreRegister(HCAE_Machine* mach, size_t core_id, HCAE_CPURegisterIndex reg_idx, HCAE_CPUWord* dst) {
    if (!mach) {
        set_error(HCAE_ERROR_II);
        return 0;
    }
    
    if (!mach->settings_applied) {
        set_error(HCAE_ERROR_ORSTBA);
        return 0;
    }
    
    return get_cpu_core_reg(mach->cpu, core_id, reg_idx, dst);
}