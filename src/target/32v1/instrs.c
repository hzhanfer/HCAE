#include <stdio.h>
#include <stdint.h>
#include "instrs.h"
#include "opcodes.h"
#include "cpu.h"
#include "cpu-run.h"


static void nop_nn(CPUCore* core, Instr* instr) {
    (void)core;
    (void)instr;
    
    printf("nop\n");
}


static void ld_rr(CPUCore* core, Instr* instr) {
    printf("ld.%d\tr%d, [r%d]\n", instr->val_len, instr->rd, instr->rs);
    
    hcae_cpu_word w = 0;
    
    if (!mem_read_arr(core->mem,
                      core->regs[instr->rs],
                      (uint8_t*)&w,
                      instr->val_len)
    ) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return;
    }
    
    core->regs[instr->rd] = w;
}


static void st_rr(CPUCore* core, Instr* instr) {
    printf("st.%d\t[r%d], r%d\n", instr->val_len, instr->rd, instr->rs);

    if (!mem_write_arr(core->mem,
                       core->regs[instr->rd],
                       (uint8_t*)&core->regs[instr->rs],
                       instr->val_len)
    ) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return;
    }
}

static void st_rv(CPUCore* core, Instr* instr) {
    printf("st.%d\t[r%d], #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    if (!mem_write_arr(core->mem,
                       core->regs[instr->rd],
                       (uint8_t*)&instr->val,
                       instr->val_len)
    ) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return;
    }
}


static void jmp_rn(CPUCore* core, Instr* instr) {
    printf("jmp\tr%d\n", instr->rs);
    
    core->regs[HCAE_REG_PC] = core->regs[instr->rs];
}

static void jmp_vn(CPUCore* core, Instr* instr) {
    printf("jmp\t#0x%x\n", instr->val);
    
    core->regs[HCAE_REG_PC] += instr->val; // PC relative jump
}


static void put_rr(CPUCore* core, Instr* instr) {
    printf("put\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] = core->regs[instr->rs];
}

static void put_rv(CPUCore* core, Instr* instr) {
    printf("put\tr%d, #0x%x\n", instr->rd, instr->val);
    
    core->regs[instr->rd] = instr->val;
}


static void jmpl_rn(CPUCore* core, Instr* instr) {
    printf("jmpl\tr%d\n", instr->rs);
    
    core->regs[HCAE_REG_LR] = core->regs[HCAE_REG_PC];
    core->regs[HCAE_REG_PC] = core->regs[instr->rs];
}

static void jmpl_vn(CPUCore* core, Instr* instr) {
    printf("jmpl\t#0x%x\n", instr->val);
    
    core->regs[HCAE_REG_LR] = core->regs[HCAE_REG_PC];
    core->regs[HCAE_REG_PC] += instr->val; // PC relative jump
}


static void ret_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    printf("ret\n");
    
    core->regs[HCAE_REG_PC] = core->regs[HCAE_REG_LR];
}

static void ret_rn(CPUCore* core, Instr* instr) {
    printf("ret\tr%d\n", instr->rs);
    
    core->regs[HCAE_REG_PC] = core->regs[instr->rs];
}


static void add_rr(CPUCore* core, Instr* instr) {
    printf("add\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] += core->regs[instr->rs];
}

static void add_rv(CPUCore* core, Instr* instr) {
    printf("add\tr%d, #0x%x\n", instr->rd, instr->val);
    
    core->regs[instr->rd] += instr->val;
}


static void sub_rr(CPUCore* core, Instr* instr) {
    printf("sub\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] -= core->regs[instr->rs];
}

static void sub_rv(CPUCore* core, Instr* instr) {
    printf("sub\tr%d, #0x%x\n", instr->rd, instr->val);
    
    core->regs[instr->rd] -= instr->val;
}


static void mul_rr(CPUCore* core, Instr* instr) {
    printf("mul\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] *= core->regs[instr->rs];
}

static void mul_rv(CPUCore* core, Instr* instr) {
    printf("mul\tr%d, #0x%x\n", instr->rd, instr->val);
    
    core->regs[instr->rd] *= instr->val;
}


static void div_rr(CPUCore* core, Instr* instr) {
    printf("div\tr%d, r%d\n", instr->rd, instr->rs);
    
    if (core->regs[instr->rs] == 0) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_DBZ);
        return;
    }
    
    core->regs[instr->rd] /= core->regs[instr->rs];
}

static void div_rv(CPUCore* core, Instr* instr) {
    printf("div\tr%d, #0x%x\n", instr->rd, instr->val);
    
    if (instr->val == 0) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_DBZ);
        return;
    }
    
    core->regs[instr->rd] /= instr->val;
}


static void sdiv_rr(CPUCore* core, Instr* instr) {
    printf("sdiv\tr%d, r%d\n", instr->rd, instr->rs);
    
    if (core->regs[instr->rs] == 0) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_DBZ);
        return;
    }
    
    hcae_cpu_sword r = core->regs[instr->rd] / core->regs[instr->rs];
    
    core->regs[instr->rd] = (hcae_cpu_word)r;
}

static void sdiv_rv(CPUCore* core, Instr* instr) {
    printf("sdiv\tr%d, #0x%x\n", instr->rd, instr->val);
    
    if (instr->val == 0) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_DBZ);
        return;
    }
    
    hcae_cpu_sword r = core->regs[instr->rd] / instr->val;
    
    core->regs[instr->rd] = r;
}


static void cact_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    
    printf("cact\n");
    
    set_pst_act(core, 0); // clear PST:ACT
}


static void scract_rn(CPUCore* core, Instr* instr) {
    printf("scract\tr%d\n", instr->rs);
    
    hcae_cpu_word w = core->regs[instr->rs];
    
    uint8_t core_id = w & 0xf; // get bits 0-3
    if (core_id >= core->cpu->core_cnt) return; // ignore invalid core id
    
    uint8_t act = (w >> 4) & 0x1; // get bit 4
    
    set_pst_act(&core->cpu->cores[core_id], act);
}


static void scrpc_rr(CPUCore* core, Instr* instr) {
    printf("scrpc\tr%d, r%d\n", instr->rd, instr->rs);
    
    uint8_t core_id = core->regs[instr->rd] & 0xf; // get bits 0-3
    if (core_id >= core->cpu->core_cnt) return; // ignore invalid core id
    
    core->cpu->cores[core_id].regs[HCAE_REG_PC] = core->regs[instr->rs]; // set PC
}


static void and_rr(CPUCore* core, Instr* instr) {
    printf("and\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] &= core->regs[instr->rs];
}

static void and_rv(CPUCore* core, Instr* instr) {
    printf("and.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] &= instr->val;
}


static void or_rr(CPUCore* core, Instr* instr) {
    printf("or\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] |= core->regs[instr->rs];
}

static void or_rv(CPUCore* core, Instr* instr) {
    printf("or.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] |= instr->val;
}


static void xor_rr(CPUCore* core, Instr* instr) {
    printf("xor\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] ^= core->regs[instr->rs];
}

static void xor_rv(CPUCore* core, Instr* instr) {
    printf("xor.%d \tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] ^= instr->val;
}


static void not_rn(CPUCore* core, Instr* instr) {
    printf("not\tr%d\n", instr->rd);
    
    core->regs[instr->rd] = ~core->regs[instr->rd];
}


static void lsl_rr(CPUCore* core, Instr* instr) {
    printf("lsl\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] <<= core->regs[instr->rs];
}

static void lsl_rv(CPUCore* core, Instr* instr) {
    printf("lsl.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] <<= instr->val;
}


static void lsr_rr(CPUCore* core, Instr* instr) {
    printf("lsr\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] >>= core->regs[instr->rs];
}

static void lsr_rv(CPUCore* core, Instr* instr) {
    printf("lsr.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] >>= instr->val;
}


static inline hcae_cpu_word asr(hcae_cpu_sword d, hcae_cpu_sword n) {
    if (n == 0) return d;
    if (n >= 32) n = 31;
    
    hcae_cpu_word r = d >> n;
    
    if (r & 0x80000000)
        r |= ~(HCAE_CPU_WORD_MAX >> n);
    
    return r;
}

static void asr_rr(CPUCore* core, Instr* instr) {
    printf("asr\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] = asr(core->regs[instr->rd], (hcae_cpu_sword)core->regs[instr->rs]);
}

static void asr_rv(CPUCore* core, Instr* instr) {
    printf("asr.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] = asr(core->regs[instr->rd], (hcae_cpu_sword)instr->val);
}


static inline hcae_cpu_word rotr(hcae_cpu_word w, hcae_cpu_word n) {
    n &= 31;
    
    return (w >> n) | (w << ((32 - n) & 31));
}

static void rotr_rr(CPUCore* core, Instr* instr) {
    printf("rotr\tr%d, r%d\n", instr->rs, instr->rd);
    
    core->regs[instr->rd] = rotr(core->regs[instr->rd], core->regs[instr->rs]);
}

static void rotr_rv(CPUCore* core, Instr* instr) {
    printf("rotr.%d\tr%d, #0x%x\n", instr->val_len, instr->rs, instr->val);
    
    core->regs[instr->rd] = rotr(core->regs[instr->rd], instr->val);
}


static void inline push(CPUCore* core, hcae_cpu_word addr, hcae_cpu_word w) {
    core->regs[HCAE_REG_SP] -= sizeof(w);
    
    if (!mem_write_arr(core->mem,
                       core->regs[HCAE_REG_SP],
                       (uint8_t*)&w,
                       sizeof(w))
    ) {
        core->regs[HCAE_REG_SP] += sizeof(w);
        core->regs[HCAE_REG_PC] = addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF);
        return;
    }
}

static void push_rn(CPUCore* core, Instr* instr) {
    printf("push\tr%d\n", instr->rs);
    
    push(core, instr->addr, core->regs[instr->rs]);
}

static void push_vn(CPUCore* core, Instr* instr) {
    printf("push.%d\t#0x%x\n", instr->val_len, instr->val);
    
    push(core, instr->addr, instr->val);
}


static void pop_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    
    printf("pop\n");
    
    core->regs[HCAE_REG_SP] += sizeof(hcae_cpu_word);
}

static void pop_rn(CPUCore* core, Instr* instr) {
    printf("pop\tr%d\n", instr->rd);
    
    if (!mem_read_arr(core->mem,
                      core->regs[HCAE_REG_SP],
                      (uint8_t*)&core->regs[instr->rd],
                      sizeof(hcae_cpu_word))
    ) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF); 
        return;
    }
    
    core->regs[HCAE_REG_SP] += sizeof(hcae_cpu_word);
}


static void sirqm_rn(CPUCore* core, Instr* instr) {
    printf("sirqm\tr%d\n", instr->rs);
    
    set_pst_irqm(core, core->regs[instr->rs]);
}

static void sirqm_vn(CPUCore* core, Instr* instr) {
    printf("sirqm.%d\t#0x%x\n", instr->val_len, instr->val);
    
    set_pst_irqm(core, instr->val);
}


instrtable instrs = {
    [OPCODE_NOP] = {[OPERORDER_NN] = nop_nn},
    [OPCODE_LD] = {[OPERORDER_RR] = ld_rr},
    [OPCODE_ST] = {[OPERORDER_RR] = st_rr, [OPERORDER_RV] = st_rv},
    [OPCODE_JMP] = {[OPERORDER_RN] = jmp_rn, [OPERORDER_VN] = jmp_vn},
    [OPCODE_PUT] = {[OPERORDER_RR] = put_rr, [OPERORDER_RV] = put_rv},
    [OPCODE_JMPL] = {[OPERORDER_RN] = jmpl_rn, [OPERORDER_VN] = jmpl_vn},
    [OPCODE_RET] = {[OPERORDER_NN] = ret_nn, [OPERORDER_RN] = ret_rn},
    [OPCODE_ADD] = {[OPERORDER_RR] = add_rr, [OPERORDER_RV] = add_rv},
    [OPCODE_SUB] = {[OPERORDER_RR] = sub_rr, [OPERORDER_RV] = sub_rv},
    [OPCODE_MUL] = {[OPERORDER_RR] = mul_rr, [OPERORDER_RV] = mul_rv},
    [OPCODE_DIV] = {[OPERORDER_RR] = div_rr, [OPERORDER_RV] = div_rv},
    [OPCODE_SDIV] = {[OPERORDER_RR] = sdiv_rr, [OPERORDER_RV] = sdiv_rv},
    [OPCODE_CACT] = {[OPERORDER_NN] = cact_nn},
    [OPCODE_SCRACT] = {[OPERORDER_RN] = scract_rn},
    [OPCODE_SCRPC] = {[OPERORDER_RR] = scrpc_rr},
    [OPCODE_AND] = {[OPERORDER_RR] = and_rr, [OPERORDER_RV] = and_rv},
    [OPCODE_OR] = {[OPERORDER_RR] = or_rr, [OPERORDER_RV] = or_rv},
    [OPCODE_XOR] = {[OPERORDER_RR] = xor_rr, [OPERORDER_RV] = xor_rv},
    [OPCODE_NOT] = {[OPERORDER_RN] = not_rn},
    [OPCODE_LSL] = {[OPERORDER_RR] = lsl_rr, [OPERORDER_RV] = lsl_rv},
    [OPCODE_LSR] = {[OPERORDER_RR] = lsr_rr, [OPERORDER_RV] = lsr_rv},
    [OPCODE_ASR] = {[OPERORDER_RR] = asr_rr, [OPERORDER_RV] = asr_rv},
    [OPCODE_ROTR] = {[OPERORDER_RR] = rotr_rr, [OPERORDER_RV] = rotr_rv},
    [OPCODE_PUSH] = {[OPERORDER_RN] = push_rn, [OPERORDER_VN] = push_vn},
    [OPCODE_POP] = {[OPERORDER_NN] = pop_nn, [OPERORDER_RN] = pop_rn},
    [OPCODE_SIRQM] = {[OPERORDER_RN] = sirqm_rn, [OPERORDER_VN] = sirqm_vn},
    [OPCODE_CMP] = {[OPCODE_RR] = cmp_rr, [OPERORDER_RV] = cmp_rv}
};
