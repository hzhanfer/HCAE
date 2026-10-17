#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "core/machine.h"
#include "cpu.h"
#include "core/hware/memory.h"
#include "core/error.h"


HCAE_Machine* HCAE_CreateMachine(void) {
    HCAE_Machine* mach = calloc(1, sizeof(HCAE_Machine));
    if (!mach) {
        set_error(HCAE_ERROR_OOM);
        return NULL;
    }
    
    CPU* cpu = create_cpu();
    if (!cpu) {
        free(mach);
        return NULL;
    }
    
    Memory* mem = create_memory();
    if (!mem) {
        destroy_cpu(cpu);
        free(mach);
        return NULL;
    }
    
    mach->cpu = cpu;
    mach->mem = mem;
    return mach;
}

void HCAE_DestroyMachine(HCAE_Machine* mach) {
    destroy_cpu(mach->cpu);
    destroy_memory(mach->mem);
    free(mach);
}


int HCAE_ApplyMachineSettings(HCAE_Machine* mach) {
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_SHABA);
        return 0;
    }
    
    if (!apply_cpu_settings(mach->cpu)) return 0;
    if (!apply_memory_settings(mach->mem)) return 0;
    
    mach->settings_applied = true;
    return 1;
}


void HCAE_SetMachineCPUDefinition(HCAE_Machine* mach, HCAE_CPUDefinition* def) {
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    mach->cpu->def = def;
}

void HCAE_SetMachineCPUCoreCount(HCAE_Machine* mach, size_t cnt) {
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    set_cpu_core_cnt(mach->cpu, cnt);
}


void HCAE_SetMachineRAM(HCAE_Machine* mach, hcae_cpu_word bot, hcae_cpu_word size) {
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    set_ram(mach->mem, bot, size);
}

void HCAE_SetMachineROM(HCAE_Machine* mach, hcae_cpu_word bot, hcae_cpu_word size) {
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    set_rom(mach->mem, bot, size);
}

void HCAE_SetMachineMMIO(HCAE_Machine* mach, hcae_cpu_word bot, hcae_cpu_word size) {
    if (mach->settings_applied) {
        set_error(HCAE_ERROR_ATCASASHABA);
        return;
    }
    
    set_mmio(mach->mem, bot, size);
}

int HCAE_SetMachineROMVectorTable(HCAE_Machine* mach, const uint8_t* table) {
    if (!mach->settings_applied) {
        set_error(HCAE_ERROR_ORSTBA);
        return 0;
    }
    
    if (!mach->mem->block) {
        set_error(HCAE_ERROR_MNAY);
        return 0;
    }
    
    memcpy(mach->mem->block, table, 16);
    
    return 1;
}


int HCAE_PrintMachine(HCAE_Machine* mach) {
    if (!mach->settings_applied) {
        set_error(HCAE_ERROR_ORSTBA);
        return 0;
    }
    
    printf(
        "Machine {\n"
        "  CPU {\n"
        "    Model {\n"
        "      Name: %s,\n"
        "      Maximum Core Count: %zu\n"
        "    },\n"
        "    Core Count: %zu,\n"
        "    Cores [\n",
        mach->cpu->def->name,
        mach->cpu->def->max_core_cnt,
        mach->cpu->core_cnt
    );
    
    for (size_t i = 0; i < mach->cpu->core_cnt; i++) {
        CPUCore* core = &mach->cpu->cores[i];
        
        printf(
            "      %zu {\n"
            "        PST:  %08x, EDAT:  %08x, ELR:  %08x,\n"
            "        PC:   %08x, LR:    %08x, SP:   %08x,\n"
            "        R00:  %08x, R01:   %08x, R02:  %08x,\n"
            "        R03:  %08x, R04:   %08x, R05:  %08x,\n"
            "        R06:  %08x, R07:   %08x, R08:  %08x,\n"
            "        R09:  %08x\n"
            "      }",
            i,
            core->sregs[HCAE_SREG_PST], core->sregs[HCAE_SREG_EDAT], core->sregs[HCAE_SREG_ELR],
            core->regs[HCAE_REG_PC], core->regs[HCAE_REG_LR], core->regs[HCAE_REG_SP],
            core->regs[0], core->regs[1], core->regs[2], core->regs[3],
            core->regs[4], core->regs[5], core->regs[6], core->regs[7],
            core->regs[8], core->regs[9]
        );
        
        if (i + 1 == mach->cpu->core_cnt) {
            printf("\n");
        } else {
            printf(",\n");
        }
    }
    
    printf(
        "    ]\n"
        "  },\n"
        "  Memory {\n"
        "    RAM: %08x-%08x %08x,\n"
        "    ROM: %08x-%08x %08x,\n"
        "    MMIO: %08x-%08x %08x,\n"
        "    Total Size: %08x\n"
        "  }\n"
        "}\n",
        mach->mem->ram.bot, mach->mem->ram.top, mach->mem->ram.size,
        mach->mem->rom.bot, mach->mem->rom.top, mach->mem->rom.size,
        mach->mem->mmio.bot, mach->mem->mmio.top, mach->mem->mmio.size,
        mach->mem->total_size
    );
    
    return 1;
}