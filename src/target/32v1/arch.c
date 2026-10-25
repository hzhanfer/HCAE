#include <string.h>

#include "arch.h"


int init_32v1(HCAE_32V1* core, HCAE_CPU* cpu, HCAE_Board* brd, HCAE_CPUModel* model) {
    core->brd = brd;
    core->cpu = cpu;
    core->model = model;

    memset(core->regs, 0, sizeof(core->regs));
    memset(core->sregs, 0, sizeof(core->sregs));

    return 1;
}

void uninit_32v1(HCAE_32V1* core) {
    (void)core;
}


int init_32v1_model(HCAE_32V1Model* model, HCAE_CPUModel* cpu_model) {
    (void)model;
    (void)cpu_model;

    return 1;
}

void uninit_32v1_model(HCAE_32V1Model* model) {
    (void)model;
}
