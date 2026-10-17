#ifndef CPU_RUN_H
#define CPU_RUN_H


#include <stdint.h>
#include "cpu.h"
#include "core/hware/memory.h"


#define set_pst_act(_core, _v) (_core)->sregs[HCAE_SREG_PST] |= (_v)
#define get_pst_act(_core) ((_core)->sregs[HCAE_SREG_PST] & 1)


enum { // operand orders
    OPER_ORDER_NN, // none none
    OPER_ORDER_RN, // reg  none
    OPER_ORDER_RR, // reg  reg
    OPER_ORDER_RV, // reg  val
    OPER_ORDER_VN  // val none
};

typedef struct CPUInstr {
    uint16_t opcode; // opcode, bit 0-10, 11 bits
    uint8_t
        oper_ord, // operand order, bit 8-10, 3 bits
        val_len, // value(imm/addr) length, bit 14-15, 2 bits
        rd, // rD, bit 16-19, 0-4 bits
        rs // rS, bit 20-24, 0-4 bits
    ;
    uint32_t val; // value, bit 16-47/22-53, 0-31 bits
} CPUInstr;


static inline int mem_read(Memory* mem, hcae_cpu_word addr, hcae_cpu_word* dst) {
    
}


#endif
