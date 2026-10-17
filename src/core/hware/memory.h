#ifndef MEMORY_H
#define MEMORY_H


#include <stdint.h>
#include "api/cpu.h"


typedef struct MemoryRegion {
    hcae_cpu_word
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
    hcae_cpu_word total_size;
    
    uint8_t* block; // allocated memory
} Memory;


Memory* create_memory(void);
void destroy_memory(Memory*);

int apply_memory_settings(Memory*);

void set_ram(Memory*, hcae_cpu_word, hcae_cpu_word);
void set_rom(Memory*, hcae_cpu_word, hcae_cpu_word);
void set_mmio(Memory*, hcae_cpu_word, hcae_cpu_word);


#endif 