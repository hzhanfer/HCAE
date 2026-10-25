#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "common/cpu.h"
#include "common/board.h"
#include "common/error.h"
#include "arch.h"


HCAE_CPU* HCAE_CreateCPU(HCAE_Board* brd, HCAE_CPUModel* model, size_t core_cnt) {
    if (!brd) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_CreateCPU", NULL, "Expected an input board, got null");
        return NULL;
    }

    if (!model) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_CreateCPU", NULL, "Expected an input model, got null");
        return NULL;
    }

    if (model->cpu_dep_cnt >= CPUMODEL_MAX_DEP_CNT) {
        set_err(HCAE_ERRCODE_MAX_EXCEEDED, "HCAE_CreateCPU", model, "There's too many CPUs that depend on the given CPU model %s", model->name);
        return NULL;
    }

    if (core_cnt == HCAE_USE_DEFAULT_CORECNT) { // use default core count
        if (model->default_core_cnt < 1) {
            set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_CreateCPU", model, "The given CPU model %s's default core count is not set", model->name);
            return NULL;
        }

        core_cnt = model->default_core_cnt;
    } else if (core_cnt < 1) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_CreateCPU", NULL, "The number of cores given to make is zero");
        return NULL;
    } else if (core_cnt > model->max_core_cnt) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS,
                "HCAE_CreateCPU",
                NULL,
                "The number of cores %zu given to make exceeds the given model %s's maximum %zu",
                core_cnt,
                model->name,
                model->max_core_cnt);
        return NULL;
    }

    if (brd->cpu) {
        set_err(HCAE_ERRCODE_BRD_ALRDY_HAS_A_CPU, "HCAE_CreateCPU", brd, "The board given already has a CPU");
        return NULL;
    }

    HCAE_CPU* cpu = malloc(sizeof(HCAE_CPU)); // create CPU
    if (!cpu) {
        set_err(HCAE_ERRCODE_OUT_OF_MEM, "HCAE_CreateCPU", NULL, "Out of memory");
        return NULL;
    }

    cpu->cores = malloc(core_cnt * sizeof(HCAE_CPUArch)); // create cores
    if (!cpu->cores) {
        set_err(HCAE_ERRCODE_OUT_OF_MEM, "HCAE_CreateCPU", NULL, "Out of memory");
        free(cpu);
        return NULL;
    }

    for (size_t i = 0; i < core_cnt; i++) { // initialize cores
        if (!init_cpu_arch(&cpu->cores[i], cpu, brd, model)) {
            for (size_t j = 0; j < i; j++) {
                uninit_cpu_arch(&cpu->cores[j]);
                return NULL;
            }
            free(cpu);
            return NULL;
        }
    }

    brd->cpu = cpu;
    cpu->brd = brd;
    cpu->model = model;
    cpu->core_cnt = core_cnt;
    model->cpu_dep_cnt++;
    return cpu;
}

int HCAE_DestroyCPU(HCAE_CPU* cpu) {
    if (!cpu) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_DestroyCPU", NULL, "Expected an input CPU, got null");
        return 0;
    }

    if (cpu->brd && HCAE_IsBoardActive(cpu->brd)) {
        set_err(HCAE_ERRCODE_OP_BLOCKED, "HCAE_DestroyCPU", cpu, "Tried to destroy an active CPU");
        return 0;
    }

    if (cpu->cores) { // uninitialize cores
        for (size_t i = 0; i < cpu->core_cnt; i++)
            uninit_cpu_arch(&cpu->cores[i]);

        free(cpu->cores);
    }

    if (cpu->brd) cpu->brd->cpu = NULL; // detach CPU from board
    if (cpu->model) cpu->model->cpu_dep_cnt--;

    free(cpu); // free CPU
    return 1;
}


HCAE_CPUModel* HCAE_CreateCPUModel(const char* name) {
    if (!name) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_CreateCPUModel", NULL, "Expected an input name, got null");
        return NULL;
    }

    if (!HCAE_IsCPUModelNameValid(name)) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS,
                "HCAE_CreateCPUModel",
                (void*)name,
                "The given CPU model name \"%s\" is inavlid",
                name);
        return NULL;
    }

    HCAE_CPUModel* model = malloc(sizeof(HCAE_CPUModel));
    if (!model) {
        set_err(HCAE_ERRCODE_OUT_OF_MEM, "HCAE_CreateCPUModel", NULL, "Out of memory");
        return NULL;
    }

    if (!init_cpu_arch_model(&model->archmodel, model)) {
        free(model);
        return NULL;
    }

    model->cpu_dep_cnt = 0;
    model->name = name;
    model->default_core_cnt = 0;
    model->max_core_cnt = 0;
    return model;
}

int HCAE_DestroyCPUModel(HCAE_CPUModel* model) {
    if (!model) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_DestroyCPUModel", NULL, "Expected an input CPU model, got null");
        return 0;
    }

    if (model->cpu_dep_cnt > 0) {
        set_err(HCAE_ERRCODE_DEP_CNT_NOT_ZERO,
                "HCAE_DestroyCPUModel",
                NULL,
                "Tried to destroy the CPU model %s while it has %zu CPU(s) that depend on it",
                model->name,
                model->cpu_dep_cnt);
        return 0;
    }

    uninit_cpu_arch_model(&model->archmodel);
    free(model);
    return 1;
}

int HCAE_SetCPUModelMaximumCoreCount(HCAE_CPUModel* model, size_t core_cnt) {
    if (!model) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_DestroyCPUModel", NULL, "Expected an input CPU model, got null");
        return 0;
    }

    if (core_cnt < 1 || core_cnt > HCAE_MAX_CORECNT) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS,
                "HCAE_SetCPUModelMaximumCoreCount",
                model,
                "Tried to set the CPU model %s's maximum core count to %zu, which is invalid, it must be greater than zero and lower or equal to %zu",
                model->name,
                core_cnt,
                HCAE_MAX_CORECNT);
        return 0;
    }

    model->max_core_cnt = core_cnt;
    return 1;
}

int HCAE_SetCPUModelDefaultCoreCount(HCAE_CPUModel* model, size_t core_cnt) {
    if (!model) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_DestroyCPUModel", NULL, "Expected an input CPU model, got null");
        return 0;
    }

    if (model->max_core_cnt < 1) {
        set_err(HCAE_ERRCODE_OP_BLOCKED,
                "HCAE_SetCPUModelDefaultCoreCount",
                model,
                "Set the CPU model %s's maximum core count first before setting the default core count",
                model->name);
        return 0;
    }

    if (core_cnt < 1 || core_cnt > model->max_core_cnt) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS,
                "HCAE_SetCPUModelDefaultCoreCount",
                model,
                "Tried to set the CPU model %s's core count to %zu, which is invalid, it must be greater than zero and lower or equal to the maximum core count %zu",
                model->name,
                core_cnt,
                model->max_core_cnt);
        return 0;
    }

    model->default_core_cnt = core_cnt;
    return 1;
}


int HCAE_IsCPUModelNameValid(const char *name)
{
    int encountered_letter = 0;
    char prev_c = '\0';

    if (name == NULL || *name == '\0' || strlen(name) < HCAE_MIN_CPUMODEL_NAME_LEN)
        return 0;

    for (; *name != '\0'; ++name) {
        char c = *name;

        if (!encountered_letter &&
            (c == ' ' || c == '-' || c == '_'))
            return 0;

        if (!((c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') ||
              c == ' ' || c == '-' || c == '_' ||
              c == '\t'))
            return 0;

        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9'))
            encountered_letter = 1;

        prev_c = c;
    }

    return prev_c != ' ' && prev_c != '-' && prev_c != '_';
}