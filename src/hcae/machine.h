#ifndef HCAE_MACHINE_H
#define HCAE_MACHINE_H


#include <stdlib.h>
#include <stdint.h>
#include HCAE_TARGET_CPU_HEADER


#define HCAE_MIN_RAM_SIZE 0x20
#define HCAE_MIN_ROM_SIZE 0x50


typedef struct HCAE_CPUModel HCAE_CPUModel; 


typedef struct HCAE_Machine HCAE_Machine;


HCAE_Machine* HCAE_CreateMachine(void);
void HCAE_DestroyMachine(HCAE_Machine*);

int HCAE_ApplyMachineSettings(HCAE_Machine*);

void HCAE_SetMachineCPUModel(HCAE_Machine*, HCAE_CPUModel*);
void HCAE_SetMachineCPUCoreCount(HCAE_Machine*, size_t);

void HCAE_SetMachineRAM(HCAE_Machine*, hcae_cpu_word, hcae_cpu_word);
void HCAE_SetMachineROM(HCAE_Machine*, hcae_cpu_word, hcae_cpu_word);
void HCAE_SetMachineMMIO(HCAE_Machine*, hcae_cpu_word, hcae_cpu_word);

int HCAE_CopyDataToMachineROM(HCAE_Machine* mach, const uint8_t*, size_t, hcae_cpu_word);

int HCAE_PrintMachine(HCAE_Machine*);

int HCAE_RunMachine(HCAE_Machine*, size_t);


#endif