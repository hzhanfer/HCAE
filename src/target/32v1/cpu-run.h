#ifndef CPU_RUN_H
#define CPU_RUN_H


#include <stdint.h>
#include "cpu.h"
#include "core/hware/memory.h"


#define PST_ACT_SHIFT 0
#define PST_IRQM_SHIFT 1
#define PST_CMP_N_SHIFT 2
#define PST_CMP_Z_SHIFT 3
#define PST_CMP_C_SHIFT 4
#define PST_CMP_V_SHIFT 5

#define PST_ACT_MASK     (0x1 << PST_ACT_SHIFT)
#define PST_IRQM_MASK    (0x1 << PST_IRQM_SHIFT)
#define PST_CMP_N_MASK   (0x1 << PST_CMP_N_SHIFT)
#define PST_CMP_Z_MASK   (0x1 << PST_CMP_Z_SHIFT)
#define PST_CMP_C_MASK   (0x1 << PST_CMP_C_SHIFT)
#define PST_CMP_V_MASK   (0x1 << PST_CMP_V_SHIFT)


typedef enum SyncExceptType { // synchronous exceptions
    SYNCEXCEPT_MF = 0, // memory fault
    SYNCEXCEPT_UI = 1, // unknown instruction
    SYNCEXCEPT_DBZ = 2, // division by zero
    SYNCEXCEPT_SC = 3 // system call
} SyncExceptType;


enum { // operand orders
    OPERORDER_NN = 0,   // none  none
    OPERORDER_RN,       // reg   none
    OPERORDER_RR,       // reg   reg
    OPERORDER_RV,       // reg   val
    OPERORDER_VN,       // val   none
    _OPERORDER_ENDOFLIST
};

typedef struct Instr {
    HCAE_CPUWord addr;
    uint16_t opcode;
    uint8_t oper_ord, val_len, rd, rs;
    uint32_t val;
} Instr;


static inline int mem_read(Memory* mem, HCAE_CPUWord addr, uint8_t* dst) {
    if (addr >= mem->total_size) return 0;
    *dst = mem->block[addr];
    return 1;
}

static inline int mem_read_arr(Memory* mem, HCAE_CPUWord addr, uint8_t* dst, HCAE_CPUWord cnt) {
    HCAE_CPUWord i;
    for (i = 0; i < cnt; addr++, i++) {
        if (addr >= mem->total_size) return 0;
        dst[i] = mem->block[addr];
    }
    return 1;
}

static inline int mem_write(Memory* mem, HCAE_CPUWord addr, HCAE_CPUWord b) {
    if (addr >= mem->total_size || (addr >= mem->rom.bot && addr < mem->rom.top))
        return 0;
    mem->block[addr] = b;
    return 1;
}

static inline int mem_write_arr(Memory* mem, HCAE_CPUWord addr, uint8_t* arr, HCAE_CPUWord cnt) {
    HCAE_CPUWord i;
    for (i = 0; i < cnt; addr++, i++) {
        if (addr >= mem->total_size || (addr >= mem->rom.bot && addr < mem->rom.top))
            return 0;
        mem->block[addr] = arr[i];
    }
    return 1;
}


void cpu_core_syncexcept(CPUCore*, SyncExceptType);


#endif
