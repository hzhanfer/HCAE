#ifndef HCAE_ERROR_H
#define HCAE_ERROR_H


typedef enum HCAE_Error {
    HCAE_ERROR_N = 0, // none
    HCAE_ERROR_OOM, // out of memory
    HCAE_ERROR_ICCC, // invalid cpu core count
    HCAE_ERROR_CDNS, // cpu definition not set
    HCAE_ERROR_ICDMCC // invalid cpu core count max core count
} HCAE_Error;


HCAE_Error HCAE_GetError(void);


#endif