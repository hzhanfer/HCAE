#ifndef HCAE_ERROR_H
#define HCAE_ERROR_H


typedef enum HCAE_Error {
    HCAE_ERROR_N = 0, // none
    HCAE_ERROR_OOM, // out of memory
    HCAE_ERROR_ORSTBA, // operation requires settings to be applied
    HCAE_ERROR_SHABA, // settings have already been applied
    HCAE_ERROR_ATCASASHABA, // attempted to change a setting after settings have already been applied
    HCAE_ERROR_ICCC, // invalid cpu core count
    HCAE_ERROR_CDNS, // cpu definition not set
    HCAE_ERROR_ICDMCC, // invalid cpu core count max core count
    HCAE_ERROR_MDNS, // memory device not set
    HCAE_ERROR_MDOWE, // memory devices overlap with eachother
    HCAE_ERROR_MDSTB, // memory device size too big
    HCAE_ERROR_MNAY, // memory not allocated yet
    HCAE_ERROR_MDITS // memory device is too small
} HCAE_Error;


HCAE_Error HCAE_GetError(void);


#endif