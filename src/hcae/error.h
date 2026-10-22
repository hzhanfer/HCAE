#ifndef HCAE_ERROR_H
#define HCAE_ERROR_H


typedef enum HCAE_Error {
    HCAE_ERROR_N = 0, // none
    HCAE_ERROR_OOM, // out of memory
    HCAE_ERROR_OOB, // out of bounds
    HCAE_ERROR_ORSTBA, // operation requires settings to be applied
    HCAE_ERROR_SHABA, // settings have already been applied
    HCAE_ERROR_ATCASASHABA, // attempted to change a setting after settings have already been applied
    HCAE_ERROR_ICCC, // invalid cpu core count
    HCAE_ERROR_IRI, // invalid register index
    HCAE_ERROR_CMNS, // cpu definition not set
    HCAE_ERROR_ICMMCC, // invalid cpu model max core count
    HCAE_ERROR_MDNS, // memory device not set
    HCAE_ERROR_MDOWE, // memory devices overlap with eachother
    HCAE_ERROR_MDSTB, // memory device size too big
    HCAE_ERROR_MDITS, // memory device is too small
    HCAE_ERROR_II, // invalid input
    HCAE_ERROR_MAR // machine already running
} HCAE_Error;


HCAE_Error HCAE_GetError(void);
void HCAE_ResetError(void);


#endif