#include <stdint.h>
#include "core/machine.h"
#include "api/cpu.h"
#include "core/error.h"


const uint8_t vectortable[HCAE_VECTORTABLE_SIZE] = {
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
    
    /* test #1 - data movement 
      // 0b00000100, 0b00100000, 0x32,   // jmp #0x32 // for test 3
    
      0b00000101, 0b00011000,            // put r2, #0x10
        0x02, 0x10,
    
      0b00000101, 0b11011000, 0x00,      // put r0, #0x12345678
        0x78, 0x56, 0x34, 0x12,
      0b00000101, 0b00010000, 0x01,      // put r1, r0
      
      0b00000101, 0b11011000,            // put r3, #0xaabbccdd
        0x03, 0xdd, 0xcc, 0xbb, 0xaa,
      0b00000101, 0b01011000, 0x04,      // put r4, #0x4000
        0x00, 0x40,
      0b00000011, 0b11010000, 0x34,      // st.4 [r4], r3
      0b00000101, 0b01011000, 0x05,      // put r5, #0x4004
        0x04, 0x40,
      0b00000011, 0b11011000, 0x05,      // st.4 [r5], #0xdeadbeef
        0xef, 0xbe, 0xad, 0xde,
    
      0b00000010, 0b11010000, 0x56,      // ld.4 r6, [r5]
      0b00000010, 0b11010000, 0x47,      // ld.4 r7, [r4]
    
      0b00000100, 0b00001000, 0x20,      // jmp r2
    */
    
    /* test 2 - arithmetic
      0b00000101, 0b00011000, 0x00,      // put r0, #0xa
        0x0a,
      0b00000101, 0b00011000, 0x01,      // put r1, #0xa
        0x0a,
      0b00001000, 0b00010000, 0x10,      // add r0, r1
    
      0b00000101, 0b00011000, 0x02,      // put r2, #0xff
        0xf0,
      0b00001001, 0b00010000, 0x12,      // sub r2, r1
    
      0b00000101, 0b00011000, 0x03,      // put r3, #0x2
        0x02,
      0b00001010, 0b00010000, 0x33,      // mul r3, r3
    
      0b00000101, 0b00011000, 0x04,      // put r4, #0x3
        0x0a,
      0b00001011, 0b00011000, 0x04,      // div r4, #0x2
        0x02,
    
      0b00001100, 0b00011000, 0x05,      // sdiv r5, #0x0 // dbz
        0x00,
    */
    
    /* test 3 - run test 1 on core 1
      0b00000101, 0b00011000, 0x00,      // put r0, #0x01
        0x01,
      0b00000101, 0b00011000, 0x01,      // put r1, #0x34
        0x13,
      0b00000101, 0b00011000, 0x02,      // put r2, #0x11
        0b00010001,
      0b00001111, 0b00010000, 0x10,      // scrpc r1, r0
      0b00001110, 0b00001000, 0x20,      // scract r2
      0b00001101, 0b00000000,            // cact
    */
    
    /* test 4 - push/pop
      0b00000101, 0b01011000,            // put sp, #0x4100
        0x0d, 0x00, 0x41,
      0b00011000, 0b11100000,            // push #0x12345678
        0x78, 0x56, 0x34, 0x12,
      0b00011000, 0b11100000,            // push #0xffffffff
        0xff, 0xff, 0xff, 0xff,
      0b00011000, 0b11100000,            // push #0x11111111
        0x11, 0x11, 0x11, 0x11,
      0b00011001, 0b00001000, 0x02,      // pop r2
      0b00011001, 0b00001000, 0x00,      // pop r0
      0b00011001, 0b00001000, 0x01,      // pop r1
    */
    
      0b00011010, 0b00100000, 0x01,      // sirqm #0x1
    
    0x00 
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
        HCAE_DestroyCPUModel(hyronicl1);
        HCAE_DestroyMachine(mach);
        return HCAE_GetError();
    }
    
    HCAE_CopyDataToMachineROM(mach, vectortable, HCAE_VECTORTABLE_SIZE, HCAE_VECTORTABLE_ADDR);
    HCAE_CopyDataToMachineROM(mach, bootrom, sizeof(bootrom), HCAE_VECTORTABLE_ADDR + HCAE_VECTORTABLE_SIZE);
    
    HCAE_RunMachine(mach, 1);
    
    HCAE_PrintMachine(mach);
    
    HCAE_DestroyCPUModel(hyronicl1);
    HCAE_DestroyMachine(mach);
    return 0;
}