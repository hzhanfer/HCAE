#include <stdint.h>
#include "core/machine.h"
#include "api/cpu.h"
#include "core/error.h"


const uint8_t vectortable[HCAE_VECTORTABLE_SIZE] = {
    HCAE_VECTORTABLE_SIZE, 0x00, 0x00, 0x00, // reset
    0x00, 0x00, 0x00, 0x00, // nmi handler
    0x00, 0x00, 0x00, 0x00, // irq handler
    0x00, 0x00, 0x00, 0x00  // synchronous exception handler
};

const uint8_t bootrom[] = {
    0xAB, 0xCD, 0xEF, 0x12,
    0x34, 0x56, 0x78, 0x89 
};


int main(void) {
    HCAE_Machine* mach = HCAE_CreateMachine();
    if (!mach) return HCAE_GetError();
    
    HCAE_CPUModel* hyronicl1 = HCAE_CreateCPUModel();
    if (!hyronicl1) {
        HCAE_DestroyMachine(mach);
        return HCAE_GetError();
    }
    
    HCAE_SetCPUModelName(hyronicl1, "Hyronic-L1");
    HCAE_SetCPUModelMaxCoreCount(hyronicl1, 2);
    
    HCAE_SetMachineCPUModel(mach, hyronicl1);
    HCAE_SetMachineCPUCoreCount(mach, 2);
    
    HCAE_SetMachineRAM(mach, 0x4000, 0x8000);
    HCAE_SetMachineROM(mach, 0x0000, 0x4000);
    HCAE_SetMachineMMIO(mach, 0xC000, 0x2000);
    
    if (!HCAE_ApplyMachineSettings(mach)) {
        HCAE_DestroyMachine(mach);
        return HCAE_GetError();
    }
    
    HCAE_CopyDataToMachineROM(mach, vectortable, HCAE_VECTORTABLE_SIZE, HCAE_VECTORTABLE_OFF);
    HCAE_CopyDataToMachineROM(mach, bootrom, sizeof(bootrom), HCAE_VECTORTABLE_OFF + HCAE_VECTORTABLE_SIZE);
    
    HCAE_PrintMachine(mach);
    
    HCAE_RunMachine(mach);
    
    HCAE_DestroyCPUModel(hyronicl1);
    HCAE_DestroyMachine(mach);
    return HCAE_GetError();
}