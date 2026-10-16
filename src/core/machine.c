#include <stdlib.h>
#include "core/machine.h"
#include "cpu.h"
#include "core/error.h"


HCAE_Machine* HCAE_CreateMachine(void) {
    HCAE_Machine* mach = calloc(1, sizeof(HCAE_Machine));
    if (!mach) {
        set_error(HCAE_ERROR_OOM);
        return NULL;
    }
    
    CPU* cpu = create_cpu();
    if (!cpu) {
        free(mach);
        return NULL;
    }
    
    mach->cpu = cpu;
    
    return mach;
}

void HCAE_DestroyMachine(HCAE_Machine* mach) {
    destroy_cpu(mach->cpu);
    free(mach);
}


int HCAE_ApplyMachineSettings(HCAE_Machine* mach) {
    if (!apply_cpu_settings(mach->cpu)) return 0;
    return 1;
}


void HCAE_SetMachineCPUDefinition(HCAE_Machine* mach, HCAE_CPUDefinition* def) {
    mach->cpu->def = def;
}

void HCAE_SetMachineCPUCoreCount(HCAE_Machine* mach, size_t cnt) {
    set_cpu_core_cnt(mach->cpu, cnt);
}