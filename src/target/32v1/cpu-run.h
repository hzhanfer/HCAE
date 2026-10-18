#ifndef CPU_RUN_H
#define CPU_RUN_H


#include <stdint.h>
#include "cpu.h"
#include "core/hware/memory.h"


#define set_pst_act(_core, _v) (_core)->sregs[HCAE_SREG_PST] |= (_v)
#define get_pst_act(_core) ((_core)->sregs[HCAE_SREG_PST] & 1)


typedef enum SyncExceptType { // synchronous exceptions
    SYNCEXCEPT_MF = 0, // memory fault
    SYNCEXCEPT_UI // unknown instruction
} SyncExceptType;


enum { // operand orders
    OPERORDER_NN = 0, // none none
    OPERORDER_RN, // reg  none
    OPERORDER_RR, // reg  reg
    OPERORDER_RV, // reg  val
    OPERORDER_VN, // val none
    _OPERORDER_ENDOFLIST
};

typedef struct Instr {
    uint16_t opcode;
    uint8_t oper_ord, val_len, rd, rs;
    uint32_t val;
} Instr;


static inline int mem_read(Memory* mem, hcae_cpu_word addr, uint8_t* dst) {
    if (addr > mem->total_size) return 0;
    *dst = mem->block[addr];
    return 1;
}

static inline int mem_read_arr(Memory* mem, hcae_cpu_word addr, uint8_t* dst, hcae_cpu_word cnt) {
    hcae_cpu_word i;
    for (i = 0; i < cnt; addr++, i++) {
        if (addr > mem->total_size) return 0;
        dst[i] = mem->block[addr];
    }
    return 1;
}

static inline int mem_write(Memory* mem, hcae_cpu_word addr, hcae_cpu_word b) {
    if (addr > mem->total_size) return 0;
    mem->block[addr] = b;
    return 1;
}

static inline int mem_write_arr(Memory* mem, hcae_cpu_word addr, uint8_t* arr, hcae_cpu_word cnt) {
    hcae_cpu_word i;
    for (i = 0; i < cnt; addr++, i++) {
        if (addr > mem->total_size) return 0;
        mem->block[addr] = arr[i];
    }
    return 1;
}


#endif
