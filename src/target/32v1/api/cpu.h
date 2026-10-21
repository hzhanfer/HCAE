#ifndef HCAE_CPU_H
#define HCAE_CPU_H


#include <stdlib.h>
#include <stdint.h>


typedef int32_t HCAE_CPUSWord;
typedef uint32_t HCAE_CPUWord;

#define HCAE_CPU_SWORD_MAX INT32_MAX
#define HCAE_CPU_SWORD_MIN INT32_MIN

#define HCAE_CPU_WORD_MAX UINT32_MAX
#define HCAE_CPU_WORD_MIN 0

#define HCAE_CPU_WORD_BITS 32
#define HCAE_CPU_SWORD_BITS 32


typedef struct HCAE_CPUModel HCAE_CPUModel;


enum {
    HCAE_SREG_PST = 0,
    HCAE_SREG_IDAT = 1,
    HCAE_SREG_ILR = 2,
    
    HCAE_REG_PC = 15,
    HCAE_REG_LR = 14,
    HCAE_REG_SP = 13
};


#define HCAE_VECTORTABLE_SIZE 16
#define HCAE_VECTORTABLE_ADDR 0x00000000
#define HCAE_VECTORTABLE_RESET_ADDR (HCAE_VECTORTABLE_ADDR + 0x00)
#define HCAE_VECTORTABLE_SYNCEXCEPT_ADDR (HCAE_VECTORTABLE_ADDR + 0x04)


HCAE_CPUModel* HCAE_CreateCPUModel(void);
void HCAE_DestroyCPUModel(HCAE_CPUModel*);

void HCAE_SetCPUModelName(HCAE_CPUModel*, const char*);
void HCAE_SetCPUModelMaxCoreCount(HCAE_CPUModel*, size_t);


#endif