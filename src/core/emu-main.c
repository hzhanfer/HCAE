#include <stdio.h>
#include <stdint.h>
#include "core/machine.h"
#include "cpu-api/cpu.h"
#include "core/error.h"


const uint8_t vecttab[HCAE_32V1_VECTTAB_SIZE] = {
    0x10, 0x00, 0x00, 0x00, // reset
    0x00, 0x00, 0x00, 0x00, // nmi handler
    0x00, 0x00, 0x00, 0x00, // irq handler
    0x00, 0x00, 0x00, 0x00  // synchronous exception handler
};

const uint8_t bootrom[] = {
    // o = opcode
    // r = operand order
    // l = value length
    
    //  oooooooo    llrrrooo
    
      0b00000101, 0b01011000, 0x0d, // put sp, #0x4100
        0x00, 0x41, 
      0b00000101, 0b11011000, 0x00, // put r0, #0x12345678
        0x78, 0x56, 0x34, 0x12,
      0b00011001, 0b00001000, 0x00, // push r0
      0b00011010, 0b00001000, 0x01, // pop r1
    
    0x00 
};


int main(void) {
    HCAE_Machine* mach = HCAE_CreateMachine();
    if (!mach) return HCAE_GetError();
    
    HCAE_32V1Model* hyronicl1 = HCAE_Create32V1Model();
    if (!hyronicl1) {
        HCAE_DestroyMachine(mach);
        return HCAE_GetError();
    }
    
    HCAE_Set32V1ModelName(hyronicl1, "Hyronic-L1");
    HCAE_Set32V1ModelDefaultCoreCount(hyronicl1, 2);
    HCAE_Set32V1ModelMaxCoreCount(hyronicl1, 2);
    
    HCAE_SetMachineCPUModel(mach, hyronicl1);
    HCAE_SetMachineCPUCoreCount(mach, HCAE_USE_DEFAULT_CPUCORECNT);
    
    HCAE_SetMachineRAM(mach, 0x4000, 0x8000);
    HCAE_SetMachineROM(mach, 0x0000, 0x4000);
    HCAE_SetMachineMMIO(mach, 0xC000, 0x2000);
    
    if (!HCAE_ApplyMachineSettings(mach)) {
        HCAE_Destroy32V1Model(hyronicl1);
        HCAE_DestroyMachine(mach);
        return HCAE_GetError();
    }
    
    HCAE_CopyDataToMachineROM(mach, vecttab, sizeof(vecttab), HCAE_32V1_VECTTAB_ADDR);
    HCAE_CopyDataToMachineROM(mach, bootrom, sizeof(bootrom), HCAE_32V1_VECTTAB_ADDR + HCAE_32V1_VECTTAB_SIZE);
    
    HCAE_RunMachine(mach, 4);
    
    HCAE_32V1Word
        regs[2][_HCAE_32V1_RR_ENDOFLIST],
        sregs[2][_HCAE_32V1_SR_ENDOFLIST]
    ;
    
    HCAE_GetMachine32V1Registers(mach, 0, regs[0]);
    HCAE_GetMachine32V1Registers(mach, 1, regs[1]);
    HCAE_GetMachine32V1SystemRegisters(mach, 0, sregs[0]);
    HCAE_GetMachine32V1SystemRegisters(mach, 1, sregs[1]);
    
    for (size_t i = 0; i < 2; i++) {
        printf("Core #%zu\n", i);
        for (size_t r = 0; r < _HCAE_32V1_RR_ENDOFLIST; r++) {
            printf("  R%02d=0x%x\n", r, regs[i][r]);
        }
        for (size_t sr = 0; sr < _HCAE_32V1_SR_ENDOFLIST; sr++) {
            printf("  SR%02d=0x%x\n", sr, sregs[i][sr]);
        }
    }
    
    HCAE_Destroy32V1Model(hyronicl1);
    HCAE_DestroyMachine(mach);
    return 0;
}