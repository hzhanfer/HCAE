#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "core/error.h"
#include "core/machine.h"
#include "core/hware/memory.h"


HCAE_CPUModel* HCAE_Create32V1Model(void) {
    HCAE_CPUModel* model = calloc(1, sizeof(HCAE_CPUModel));
    if (!model) {
        set_error(HCAE_ERROR_OOM);
        return NULL;
    }
    
    return model;
}

void HCAE_Destroy32V1Model(HCAE_CPUModel* model) {
    free(model);
}


void HCAE_Set32V1ModelName(HCAE_CPUModel* model, const char* name) {
    model->name = name;
}

void HCAE_Set32V1ModelMaxCoreCount(HCAE_CPUModel* model, size_t cnt) {
    model->max_core_cnt = cnt;
}


void set_cpu_model(CPU* cpu, HCAE_CPUModel* model) {
    cpu->model = model;
}

void set_cpu_core_cnt(CPU* cpu, size_t cnt) {
    cpu->core_cnt = cnt;
}


CPU* create_cpu(HCAE_Machine* mach) {
    CPU* cpu = calloc(1, sizeof(CPU));
    if (!cpu) return NULL;
    
    cpu->mach = mach;
    
    return cpu;
}

void destroy_cpu(CPU* cpu) {
    if (cpu->cores) free(cpu->cores);
    free(cpu);
}


int get_cpu_core_reg(CPU* cpu, size_t core_id, HCAE_32V1RegisterIndex reg_idx, HCAE_CPUWord* dst) {
    if (core_id >= cpu->core_cnt) {
        set_error(HCAE_ERROR_ICCC);
        return 0;
    }
    
    if (reg_idx >= _HCAE_32V1_RR_ENDOFLIST) {
        set_error(HCAE_ERROR_IRI);
        return 0;
    }
    
    *dst = cpu->cores[core_id].regs[]
    return 1;
}


int apply_cpu_settings(CPU* cpu) {
    if (!cpu->model) {
        set_error(HCAE_ERROR_CMNS);
        return 0;
    }
    
    if (cpu->model->max_core_cnt < 1) {
        set_error(HCAE_ERROR_ICMMCC);
        return 0;
    }
    
    if (cpu->core_cnt < 1 || cpu->core_cnt > cpu->model->max_core_cnt) {
        set_error(HCAE_ERROR_ICCC);
        return 0;
    }
    
    cpu->cores = calloc(cpu->core_cnt, sizeof(CPUCore));
    if (!cpu->cores) {
        set_error(HCAE_ERROR_OOM);
        return 0;
    }
    
    for (size_t i = 0; i < cpu->core_cnt; i++) {
        CPUCore* core = &cpu->cores[i];
        
        core->cpu = cpu;
        core->mem = cpu->mach->mem;
    }
    
    return 1;
}

