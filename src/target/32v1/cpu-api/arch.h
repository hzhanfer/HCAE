#ifndef __HCAE_HEADER_32V1
#define __HCAE_HEADER_32V1


#include <stdint.h>


typedef struct HCAE_32V1 HCAE_32V1;
typedef struct HCAE_32V1Model HCAE_32V1Model;

#define HCAE_CPUArch HCAE_32V1
#define HCAE_CPUArchModel HCAE_32V1Model


typedef uint32_t HCAE_32V1Word;
typedef HCAE_32V1Word HCAE_CPUWord;


typedef enum HCAE_32V1RegisterIndex {
    HCAE_32V1_SP = 13, // stack pointer
    HCAE_32V1_LR, // link register
    HCAE_32V1_PC, // program counter
    _HCAE_32V1_REG_ENDOFLIST,

    HCAE_32V1_CSTATE = 0, // core state
    HCAE_32V1_ILR, // interrupt link register
    _HCAE_32V1_SREG_ENDOFLIST
} HCAE_32V1RegisterIndex;


#endif