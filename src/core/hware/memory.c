#include <stdlib.h>
#include "core/hware/memory.h"
#include "cpu.h"
#include "core/machine.h"
#include "core/error.h"


Memory* create_memory(void) {
    Memory* mem = calloc(1, sizeof(Memory));
    if (!mem) return NULL;
    
    return mem;
}

void destroy_memory(Memory* mem) {
    if (mem->block) free(mem->block);
    free(mem);
}


static inline int mem_rg_overlap(MemoryRegion* r1, MemoryRegion* r2) {
    return r1->bot < r2->top && r2->bot < r1->top;
}

int apply_memory_settings(Memory* mem) {
    if (mem->ram.size == 0 || mem->rom.size == 0) {
        set_error(HCAE_ERROR_MDNS);
        return 0;
    }
    
    if (mem->ram.size < HCAE_MIN_RAM_SIZE || mem->rom.size < HCAE_MIN_ROM_SIZE) {
        set_error(HCAE_ERROR_MDITS);
        return 0;
    }
    
    if (
        mem_rg_overlap(&mem->ram, &mem->rom) ||
        mem_rg_overlap(&mem->ram, &mem->mmio) ||
        mem_rg_overlap(&mem->rom, &mem->mmio)
    ) {
        set_error(HCAE_ERROR_MDOWE);
        return 0;
    }
    
    HCAE_CPUWord total_size = mem->ram.size + mem->rom.size;
    
    if (total_size < mem->ram.size) {
        set_error(HCAE_ERROR_MDSTB);
        return 0;
    }
    
    total_size += mem->mmio.size;
    
    if (total_size < mem->ram.size + mem->rom.size) {
        set_error(HCAE_ERROR_MDSTB);
        return 0;
    }
    
    mem->block = malloc((size_t)total_size);
    if (!mem->block) {
        set_error(HCAE_ERROR_OOM);
        return 0;
    }
    
    mem->total_size = total_size;
    
    return 1;
}


void set_ram(Memory* mem, HCAE_CPUWord bot, HCAE_CPUWord size) {
    mem->ram.top = bot + size;
    mem->ram.bot = bot;
    mem->ram.size = size;
}

void set_rom(Memory* mem, HCAE_CPUWord bot, HCAE_CPUWord size) {
    mem->rom.top = bot + size;
    mem->rom.bot = bot;
    mem->rom.size = size;
}

void set_mmio(Memory* mem, HCAE_CPUWord bot, HCAE_CPUWord size) {
    mem->mmio.top = bot + size;
    mem->mmio.bot = bot;
    mem->mmio.size = size;
}