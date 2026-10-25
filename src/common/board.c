#include <stdlib.h>

#include "common/board.h"
#include "common/error.h"


HCAE_Board* HCAE_CreateBoard(void) {
    HCAE_Board* brd = malloc(sizeof(HCAE_Board));
    if (!brd) {
        set_err(HCAE_ERRCODE_OUT_OF_MEM, "HCAE_CreateBoard", NULL, "Could not create board");
        return NULL;
    }

    brd->activity = BRD_ACT_INACTIVE;
    brd->cpu = NULL;
    return brd;
}

int HCAE_DestroyBoard(HCAE_Board* brd) {
    if (!brd) {
        set_err(HCAE_ERRCODE_INVALID_PARAMS, "HCAE_DestroyBoard", NULL, "Expected an input board, got null");
        return 0;
    }

    if (HCAE_IsBoardActive(brd)) {
        set_err(HCAE_ERRCODE_OP_BLOCKED, "HCAE_DestroyBoard", brd, "Tried to destroy an active board");
        return 0;
    }

    free(brd);
    return 1;
}


int HCAE_IsBoardActive(HCAE_Board* brd) {
    return brd->activity != BRD_ACT_INACTIVE && brd->activity != BRD_ACT_FINISHEDRUN;
}