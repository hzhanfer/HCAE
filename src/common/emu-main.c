#include <stdio.h>

#include "hcae/board.h"
#include "hcae/cpu.h"
#include "hcae/error.h"


int main(void) {
    HCAE_Board* brd = HCAE_CreateBoard();
    if (!brd) {
        fprintf(stderr, "%d:%s:%p: %s\n", HCAE_GetErrorCode(), HCAE_GetErrorFunctionName(), HCAE_GetErrorObject(), HCAE_GetErrorMessage());
        return 1;
    }

    HCAE_CPUModel* hyronicl1 = HCAE_CreateCPUModel("Hyronic-L1");
    if (!hyronicl1) {
        fprintf(stderr, "%d:%s:%p: %s\n", HCAE_GetErrorCode(), HCAE_GetErrorFunctionName(), HCAE_GetErrorObject(), HCAE_GetErrorMessage());
        HCAE_DestroyBoard(brd);
        return 1;
    }

    HCAE_SetCPUModelMaximumCoreCount(hyronicl1, 2);
    HCAE_SetCPUModelDefaultCoreCount(hyronicl1, 1);

    HCAE_CPU* cpu = HCAE_CreateCPU(brd, hyronicl1, 2);
    if (!cpu) {
        fprintf(stderr, "%d:%s:%p: %s\n", HCAE_GetErrorCode(), HCAE_GetErrorFunctionName(), HCAE_GetErrorObject(), HCAE_GetErrorMessage());
        HCAE_DestroyCPUModel(hyronicl1);
        HCAE_DestroyBoard(brd);
        return 1;
    }

    HCAE_DestroyCPU(cpu);
    HCAE_DestroyCPUModel(hyronicl1);
    HCAE_DestroyBoard(brd);
    return 0;
}
