#ifndef HCAE_MACHINE_H
#define HCAE_MACHINE_H


#include <stdlib.h>


typedef struct HCAE_CPUDefinition HCAE_CPUDefinition; 


typedef struct HCAE_Machine HCAE_Machine;


HCAE_Machine* HCAE_CreateMachine(void);
void HCAE_DestroyMachine(HCAE_Machine*);

int HCAE_ApplyMachineSettings(HCAE_Machine*);

void HCAE_SetMachineCPUDefinition(HCAE_Machine*, HCAE_CPUDefinition*);
void HCAE_SetMachineCPUCoreCount(HCAE_Machine*, size_t);


#endif