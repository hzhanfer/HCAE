#include "core/machine.h"
#include "cpu.h"
#include "core/error.h"


int main(void) {
    HCAE_Machine* mach = HCAE_CreateMachine();
    if (!mach) return HCAE_GetError();
    
    HCAE_CPUDefinition* hyronicl1 = HCAE_CreateCPUDefinition();
    if (!hyronicl1) {
        HCAE_DestroyMachine(mach);
        return HCAE_GetError();
    }
    
    HCAE_SetCPUDefinitionName(hyronicl1, "Hyronic-L1");
    HCAE_SetCPUDefinitionMaxCoreCount(hyronicl1, 2);
    
    HCAE_SetMachineCPUDefinition(mach, hyronicl1);
    HCAE_SetMachineCPUCoreCount(mach, 2);
    
    if (!HCAE_ApplyMachineSettings(mach)) {
        HCAE_DestroyMachine(mach);
        return HCAE_GetError();
    }
    
    HCAE_DestroyCPUDefinition(hyronicl1);
    HCAE_DestroyMachine(mach);
    return 0;
}