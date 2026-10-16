#include <stdlib.h>
#include "cpu.h"
#include "core/error.h"


HCAE_CPUDefinition* HCAE_CreateCPUDefinition(void) {
    HCAE_CPUDefinition* def = calloc(1, sizeof(HCAE_CPUDefinition));
    if (!def) {
        set_error(HCAE_ERROR_OOM);
        return NULL;
    }
    
    return def;
}

void HCAE_DestroyCPUDefinition(HCAE_CPUDefinition* def) {
    free(def);
}


void HCAE_SetCPUDefinitionName(HCAE_CPUDefinition* def, const char* name) {
    def->name = name;
}

void HCAE_SetCPUDefinitionMaxCoreCount(HCAE_CPUDefinition* def, size_t cnt) {
    def->max_core_cnt = cnt;
}


static inline int init_cpu_core(CPUCore* core) {
    (void)core;
    return 1;
}


void set_cpu_def(CPU* cpu, HCAE_CPUDefinition* def) {
    cpu->def = def;
}

void set_cpu_core_cnt(CPU* cpu, size_t cnt) {
    cpu->core_cnt = cnt;
}


CPU* create_cpu(void) {
    CPU* cpu = calloc(1, sizeof(CPU));
    if (!cpu) return NULL;
    
    return cpu;
}

void destroy_cpu(CPU* cpu) {
    if (cpu->cores) free(cpu->cores);
    free(cpu);
}


int apply_cpu_settings(CPU* cpu) {
    if (!cpu->def) {
        set_error(HCAE_ERROR_CDNS);
        return 0;
    }
    
    if (cpu->def->max_core_cnt < 1) {
        set_error(HCAE_ERROR_ICDMCC);
        return 0;
    }
    
    if (cpu->core_cnt < 1 || cpu->core_cnt > cpu->def->max_core_cnt) {
        set_error(HCAE_ERROR_ICCC);
        return 0;
    }
    
    cpu->cores = calloc(cpu->core_cnt, sizeof(CPUCore));
    if (!cpu->cores) {
        set_error(HCAE_ERROR_OOM);
        return 0;
    }
    
    for (size_t i = 0; i < cpu->core_cnt; i++) {
        if (!init_cpu_core(&cpu->cores[i])) {
            free(cpu->cores);
            return 0;
        }
    }
    
    return 1;
}