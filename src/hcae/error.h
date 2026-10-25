#ifndef __HCAE_HEADER_ERROR
#define __HCAE_HEADER_ERROR


typedef enum HCAE_ErrorCode {
    HCAE_ERRCODE_NONE = 0,
    HCAE_ERRCODE_OUT_OF_MEM,
    HCAE_ERRCODE_INVALID_PARAMS,
    HCAE_ERRCODE_OP_BLOCKED,
    HCAE_ERRCODE_MAX_EXCEEDED,
    HCAE_ERRCODE_DEP_CNT_NOT_ZERO,
    HCAE_ERRCODE_BRD_ALRDY_HAS_A_CPU
} HCAE_ErrorCode;


HCAE_ErrorCode HCAE_GetErrorCode(void);
const char* HCAE_GetErrorFunctionName(void);
const char* HCAE_GetErrorMessage(void);
void* HCAE_GetErrorObject(void);

void HCAE_ClearError(void);


#endif