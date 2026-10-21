#ifndef MEMORY_H
#define MEMORY_H


#include <stdint.h>
#include "api/cpu.h"


typedef struct MemoryRegion {
    HCAE_CPUWord
        top,
        bot,
        size
    ;
} MemoryRegion;


typedef struct Memory {
    MemoryRegion
        ram,
        rom,
        mmio
    ;
    HCAE_CPUWord total_size;
    
    uint8_t* block; // allocated memory
} Memory;


Memory* create_memory(void);
void destroy_memory(Memory*);

int apply_memory_settings(Memory*);

void set_ram(Memory*, HCAE_CPUWord, HCAE_CPUWord);
void set_rom(Memory*, HCAE_CPUWord, HCAE_CPUWord);
void set_mmio(Memory*, HCAE_CPUWord, HCAE_CPUWord);


#endif 