#ifndef CPU_RUN_H
#define CPU_RUN_H


#include <stdint.h>
#include "cpu.h"
#include "core/hware/memory.h"
#include "cpu-api/opcodes.h"


typedef struct Instr {
    HCAE_32V1Word addr;
    HCAE_32V1Opcode opcode;
    HCAE_32V1OperandOrder oper_ord;
    uint8_t val_len;
    HCAE_32V1RegisterIndex rd, rs;
    uint32_t val;
} Instr;


static inline int mem_read(Memory* mem, HCAE_32V1Word addr, uint8_t* dst) {
    if (addr >= mem->total_size) return 0;
    *dst = mem->block[addr];
    return 1;
}

static inline int mem_read_arr(Memory* mem, HCAE_32V1Word addr, uint8_t* dst, HCAE_CPUWord cnt) {
    HCAE_32V1Word i;
    for (i = 0; i < cnt; addr++, i++) {
        if (addr >= mem->total_size) return 0;
        dst[i] = mem->block[addr];
    }
    return 1;
}

static inline int mem_write(Memory* mem, HCAE_32V1Word addr, HCAE_32V1Word b) {
    if (addr >= mem->total_size || (addr >= mem->rom.bot && addr < mem->rom.top))
        return 0;
    mem->block[addr] = b;
    return 1;
}

static inline int mem_write_arr(Memory* mem, HCAE_32V1Word addr, uint8_t* arr, HCAE_32V1Word cnt) {
    HCAE_32V1Word i;
    for (i = 0; i < cnt; addr++, i++) {
        if (addr >= mem->total_size || (addr >= mem->rom.bot && addr < mem->rom.top))
            return 0;
        mem->block[addr] = arr[i];
    }
    return 1;
}


void cpu_core_syncexcept(CPUCore*, HCAE_32V1SyncException);


#endif
