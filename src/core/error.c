#include "hcae/error.h"


static HCAE_Error err = HCAE_ERROR_N;



void set_error(HCAE_Error e) {
    err = e;
}

HCAE_Error HCAE_GetError(void) {
    return err;
}

void HCAE_ResetError(void) {
    err = HCAE_ERROR_N;
}