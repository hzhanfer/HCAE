#ifndef HCAE_32V1_H
#define HCAE_32V1_H


#include <stdlib.h>
#include <stdint.h>


typedef int32_t HCAE_32V1SWord;
typedef uint32_t HCAE_32V1Word;

typedef HCAE_32V1SWord HCAE_CPUSWord;
typedef HCAE_32V1Word HCAE_CPUWord;


#define HCAE_32V1_SWORD_MAX INT32_MAX
#define HCAE_32V1_SWORD_MIN INT32_MIN
#define HCAE_32V1_WORD_MAX UINT32_MAX
#define HCAE_32V1_WORD_MIN 0
#define HCAE_32V1_WORD_BITS 32
#define HCAE_32V1_SWORD_BITS 32

#define HCAE_32V1_PST.ACT_SHIFT 0
#define HCAE_32V1_PST_IRQM_SHIFT 1
#define HCAE_32V1_PST_CMP_N_SHIFT 2
#define HCAE_32V1_PST_CMP_Z_SHIFT 3
#define HCAE_32V1_PST_CMP_C_SHIFT 4
#define HCAE_32V1_PST_CMP_V_SHIFT 5

#define HCAE_32V1_PST.ACT_MASK     (0x1 << HCAE_32V1_PST.ACT_SHIFT)
#define HCAE_32V1_PST_IRQM_MASK    (0x1 << HCAE_32V1_PST_IRQM_SHIFT)
#define HCAE_32V1_PST_CMP_N_MASK   (0x1 << HCAE_32V1_PST_CMP_N_SHIFT)
#define HCAE_32V1_PST_CMP_Z_MASK   (0x1 << HCAE_32V1_PST_CMP_Z_SHIFT)
#define HCAE_32V1_PST_CMP_C_MASK   (0x1 << HCAE_32V1_PST_CMP_C_SHIFT)
#define HCAE_32V1_PST_CMP_V_MASK   (0x1 << HCAE_32V1_PST_CMP_V_SHIFT)


typedef enum HCAE_32V1SyncException { // synchronous exceptions
    HCAE_32V1_SYNCEXCEPT_MF = 0, // memory fault
    HCAE_32V1_SYNCEXCEPT_UI = 1, // unknown instruction
    HCAE_32V1_SYNCEXCEPT_DBZ = 2, // division by zero
    HCAE_32V1_SYNCEXCEPT_SC = 3 // system call
} HCAE_32V1SyncException;

typedef enum HCAE_32V1OperandOrder { // operand orders
    HCAE_32V1_OPERORDER_NN = 0,   // none  none
    HCAE_32V1_OPERORDER_RN,       // reg   none
    HCAE_32V1_OPERORDER_RR,       // reg   reg
    HCAE_32V1_OPERORDER_RV,       // reg   val
    HCAE_32V1_OPERORDER_VN,       // val   none
    _HCAE_32V1_OPERORDER_ENDOFLIST
} HCAE_32V1OperandOrder;


typedef struct HCAE_32V1Model HCAE_32V1Model;

#define HCAE_CPUModel HCAE_32V1Model


typedef enum HCAE_32V1RegisterIndex {
    // special-purpose registers
    HCAE_32V1_SPR_SP = 13,
    HCAE_32V1_SPR_PC,
    HCAE_32V1_SPR_LR,
    _HCAE_32V1_RR_ENDOFLIST, // end of regular-registers list (includes SPRs)
    
    // system registers
    HCAE_32V1_SR_PST = 0,
    HCAE_32V1_SR_IDAT,
    HCAE_32V1_SR_ILR,
    _HCAE_32V1_SR_ENDOFLIST,
} HCAE_32V1RegisterIndex;


typedef HCAE_32V1RegisterIndex HCAE_CPURegisterIndex;


#define HCAE_32V1_VECTTAB_SIZE 16
#define HCAE_32V1_VECTTAB_ADDR 0x00000000
#define HCAE_32V1_VECTTAB_RESET_ADDR (HCAE_32V1_VECTTAB_ADDR + 0x00)
#define HCAE_32V1_VECTTAB_SYNCEXCEPT_ADDR (HCAE_32V1_VECTTAB_ADDR + 0x04)


HCAE_32V1Model* HCAE_Create32V1Model(void);
void HCAE_Destroy32V1Model(HCAE_32V1Model*);

void HCAE_Set32V1ModelName(HCAE_32V1Model*, const char*);
void HCAE_Set32V1ModelMaxCoreCount(HCAE_32V1Model*, size_t);


#endif