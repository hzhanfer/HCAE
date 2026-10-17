#include <stdint.h>
#define HCAE_TARGET_CPU_HEADER "api/cpu.h"
#include "hcae/machine.h"
#include "api/cpu.h"
#include "hcae/error.h"


const uint8_t vectortable[16] = {
    0x10, 0x00, 0x00, 0x00, // reset
    0x00, 0x00, 0x00, 0x00, // nmi handler
    0x00, 0x00, 0x00, 0x00, // irq handler
    0x00, 0x00, 0x00, 0x00  // synchronous exception handler
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
    
    HCAE_SetMachineROMVectorTable(mach, vectortable);
    
    HCAE_PrintMachine(mach);
    
    HCAE_DestroyCPUModel(hyronicl1);
    HCAE_DestroyMachine(mach);
    return HCAE_GetError();
}