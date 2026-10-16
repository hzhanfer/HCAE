#ifndef HCAE_CPU_H
#define HCAE_CPU_H


#include <stdlib.h>
#include <stdint.h>


typedef uint32_t hcae_cpu_word;


typedef struct HCAE_CPUDefinition HCAE_CPUDefinition;


HCAE_CPUDefinition* HCAE_CreateCPUDefinition(void);
void HCAE_DestroyCPUDefinition(HCAE_CPUDefinition*);

void HCAE_SetCPUDefinitionName(HCAE_CPUDefinition*, const char*);
void HCAE_SetCPUDefinitionMaxCoreCount(HCAE_CPUDefinition*, size_t);


#endif