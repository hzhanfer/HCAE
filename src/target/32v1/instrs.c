#include <stdint.h>
#include "instrs.h"
#include "cpu-api/opcodes.h"
#include "cpu.h"
#include "cpu-run.h"


static void nop_nn(CPUCore* core, Instr* instr) {
    (void)core;
    (void)instr;
}


static void ld_rr(CPUCore* core, Instr* instr) {
    HCAE_32V1Word w = 0;
    
    if (!mem_read_arr(core->mem,
                      core->regs[instr->rs],
                      (uint8_t*)&w,
                      instr->val_len)
    ) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF);
        return;
    }
    
    core->regs[instr->rd] = w;
}


static void st_rr(CPUCore* core, Instr* instr) {
    if (!mem_write_arr(core->mem,
                       core->regs[instr->rd],
                       (uint8_t*)&core->regs[instr->rs],
                       instr->val_len)
    ) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF);
        return;
    }
}

static void st_rv(CPUCore* core, Instr* instr) {
    if (!mem_write_arr(core->mem,
                       core->regs[instr->rd],
                       (uint8_t*)&instr->val,
                       instr->val_len)
    ) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF);
        return;
    }
}


static void j_rn(CPUCore* core, Instr* instr) {
    core->regs[HCAE_32V1_PC] = core->regs[instr->rs];
}

static void j_vn(CPUCore* core, Instr* instr) {
    core->regs[HCAE_32V1_PC] += instr->val; // PC relative jump
}


static void put_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = core->regs[instr->rs];
}

static void put_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = instr->val;
}


static void jl_rn(CPUCore* core, Instr* instr) {
    core->regs[HCAE_32V1_LR] = core->regs[HCAE_32V1_PC];
    core->regs[HCAE_32V1_PC] = core->regs[instr->rs];
}

static void jl_vn(CPUCore* core, Instr* instr) {
    core->regs[HCAE_32V1_LR] = core->regs[HCAE_32V1_PC];
    core->regs[HCAE_32V1_PC] += instr->val; // PC relative jump
}


static void ret_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    core->regs[HCAE_32V1_PC] = core->regs[HCAE_32V1_LR];
}

static void ret_rn(CPUCore* core, Instr* instr) {
    core->regs[HCAE_32V1_PC] = core->regs[instr->rs];
}


static void add_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] += core->regs[instr->rs];
}

static void add_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] += instr->val;
}


static void sub_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] -= core->regs[instr->rs];
}

static void sub_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] -= instr->val;
}


static void mul_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] *= core->regs[instr->rs];
}

static void mul_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] *= instr->val;
}


static void div_rr(CPUCore* core, Instr* instr) {
    if (core->regs[instr->rs] == 0) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_DBZ);
        return;
    }
    
    core->regs[instr->rd] /= core->regs[instr->rs];
}

static void div_rv(CPUCore* core, Instr* instr) {
    if (instr->val == 0) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_DBZ);
        return;
    }
    
    core->regs[instr->rd] /= instr->val;
}


static void sdiv_rr(CPUCore* core, Instr* instr) {
    if (core->regs[instr->rs] == 0) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_DBZ);
        return;
    }
    
    HCAE_32V1SWord r = core->regs[instr->rd] / core->regs[instr->rs];
    
    core->regs[instr->rd] = (HCAE_32V1Word)r;
}

static void sdiv_rv(CPUCore* core, Instr* instr) {
    if (instr->val == 0) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_DBZ);
        return;
    }
    
    HCAE_32V1SWord r = core->regs[instr->rd] / instr->val;
    
    core->regs[instr->rd] = r;
}


static void cact_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    
    core->sregs[HCAE_32V1_PST] &= ~HCAE_32V1_PST_ACT_MASK;
}


static void scract_rn(CPUCore* core, Instr* instr) {
    HCAE_32V1Word w = core->regs[instr->rs];
    
    uint8_t core_id = w & 0xf; // get bits 0-3
    if (core_id >= core->cpu->core_cnt) return; // ignore invalid core id
    
    uint8_t act = (w >> 4) & 0x1; // get bit 4
    
    core->sregs[HCAE_32V1_PST] = (core->regs[HCAE_32V1_PST] & ~HCAE_32V1_PST_ACT_MASK) | (act << HCAE_32V1_PST_ACT_SHIFT);
}


static void scrpc_rr(CPUCore* core, Instr* instr) {
    uint8_t core_id = core->regs[instr->rd] & 0xf; // get bits 0-3
    if (core_id >= core->cpu->core_cnt) return; // ignore invalid core id
    
    core->cpu->cores[core_id].regs[HCAE_32V1_PC] = core->regs[instr->rs]; // set PC
}


static void and_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] &= core->regs[instr->rs];
}

static void and_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] &= instr->val;
}


static void or_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] |= core->regs[instr->rs];
}

static void or_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] |= instr->val;
}


static void xor_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] ^= core->regs[instr->rs];
}

static void xor_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] ^= instr->val;
}


static void not_rn(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = ~core->regs[instr->rd];
}


static void lsl_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] <<= core->regs[instr->rs];
}

static void lsl_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] <<= instr->val;
}


static void lsr_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] >>= core->regs[instr->rs];
}

static void lsr_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] >>= instr->val;
}


static inline HCAE_32V1Word asr(HCAE_32V1SWord d, HCAE_32V1SWord n) {
    if (n == 0) return d;
    if (n >= 32) n = 31;
    
    HCAE_32V1Word r = d >> n;
    
    if (r & 0x80000000)
        r |= ~(HCAE_32V1_WORD_MAX >> n);
    
    return r;
}

static void asr_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = asr(core->regs[instr->rd], (HCAE_32V1SWord)core->regs[instr->rs]);
}

static void asr_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = asr(core->regs[instr->rd], (HCAE_32V1SWord)instr->val);
}


static inline HCAE_32V1Word rotr(HCAE_32V1Word w, HCAE_32V1Word n) {
    n &= HCAE_32V1_WORD_BITS - 1;
    
    return (w >> n) | (w << ((HCAE_32V1_WORD_BITS - n) & (HCAE_32V1_WORD_BITS - 1)));
}

static void rotr_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = rotr(core->regs[instr->rd], core->regs[instr->rs]);
}

static void rotr_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = rotr(core->regs[instr->rd], instr->val);
}


static void inline push(CPUCore* core, HCAE_32V1Word addr, HCAE_32V1Word w) {
    core->regs[HCAE_32V1_SP] -= sizeof(w);
    
    if (!mem_write_arr(core->mem,
                       core->regs[HCAE_32V1_SP],
                       (uint8_t*)&w,
                       sizeof(w))
    ) {
        core->regs[HCAE_32V1_SP] += sizeof(w);
        core->regs[HCAE_32V1_PC] = addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF);
        return;
    }
}

static void push_rn(CPUCore* core, Instr* instr) {
    push(core, instr->addr, core->regs[instr->rs]);
}

static void push_vn(CPUCore* core, Instr* instr) {
    push(core, instr->addr, instr->val);
}


static void pop_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    
    core->regs[HCAE_32V1_SP] += sizeof(HCAE_32V1Word);
}

static void pop_rn(CPUCore* core, Instr* instr) {
    if (!mem_read_arr(core->mem,
                      core->regs[HCAE_32V1_SP],
                      (uint8_t*)&core->regs[instr->rd],
                      sizeof(HCAE_32V1Word))
    ) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_MF); 
        return;
    }
    
    core->regs[HCAE_32V1_SP] += sizeof(HCAE_32V1Word);
}


static void sirqm_rn(CPUCore* core, Instr* instr) {
    core->sregs[HCAE_32V1_PST] = (core->sregs[HCAE_32V1_PST] & ~HCAE_32V1_PST_IRQM_MASK) | ((core->regs[instr->rs] & 0x1) << HCAE_32V1_PST_IRQM_SHIFT);
}

static void sirqm_vn(CPUCore* core, Instr* instr) {
    core->sregs[HCAE_32V1_PST] = (core->sregs[HCAE_32V1_PST] & ~HCAE_32V1_PST_IRQM_MASK) | ((instr->val & 0x1) << HCAE_32V1_PST_IRQM_SHIFT);
}


static inline HCAE_32V1Word cmp(CPUCore* core, HCAE_32V1Word a, HCAE_32V1Word b) {
    HCAE_32V1Word r = a - b;
    
    HCAE_32V1Word pst = core->sregs[HCAE_32V1_PST];

    pst &= ~(HCAE_32V1_PST_CONDF_N_MASK | HCAE_32V1_PST_CONDF_Z_MASK | HCAE_32V1_PST_CONDF_C_MASK | HCAE_32V1_PST_CONDF_V_MASK);
    
    pst |= (((r >> (HCAE_32V1_WORD_BITS - 1)) & 0x1) << HCAE_32V1_PST_CONDF_N_SHIFT);
    pst |= ((r == 0) << HCAE_32V1_PST_CONDF_Z_SHIFT);
    pst |= ((a >= b) << HCAE_32V1_PST_CONDF_C_SHIFT);
    pst |= ((((a ^ b) & (a ^ r)) >> (HCAE_32V1_WORD_BITS - 1) & 0x1) << HCAE_32V1_PST_CONDF_V_SHIFT);
    
    core->sregs[HCAE_32V1_PST] = pst;
    
    return r;
}

static void cmp_rr(CPUCore* core, Instr* instr) {
    cmp(core, core->regs[instr->rd], core->regs[instr->rs]);
}

static void cmp_rv(CPUCore* core, Instr* instr) {
    cmp(core, core->regs[instr->rd], instr->val);
}


static void jlo_vn(CPUCore* core, Instr* instr) {
    if ((core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_C_MASK) == 0)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jhs_vn(CPUCore* core, Instr* instr) {
    if (core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_C_MASK)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jhi_vn(CPUCore* core, Instr* instr) {
    uint8_t
        c = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_C_MASK,
        z = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_Z_MASK
    ;
    
    if (c == 1 && z == 0)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jls_vn(CPUCore* core, Instr* instr) {
    uint8_t
        c = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_C_MASK,
        z = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_Z_MASK
    ;
    
    if (c == 0 || z == 1)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jlt_vn(CPUCore* core, Instr* instr) {
    uint8_t
        n = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_N_MASK,
        v = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_V_MASK
    ;
    
    if (n != v)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jge_vn(CPUCore* core, Instr* instr) {
    uint8_t
        n = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_N_MASK,
        v = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_V_MASK
    ;
    
    if (n == v)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jgt_vn(CPUCore* core, Instr* instr) {
    uint8_t
        n = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_N_MASK,
        v = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_V_MASK,
        z = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_Z_MASK
    ;
    
    if (z == 0 && n == v)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jle_vn(CPUCore* core, Instr* instr) {
    uint8_t
        n = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_N_MASK,
        v = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_V_MASK,
        z = core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_Z_MASK
    ;
    
    if (z == 1 && n != v)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jeq_vn(CPUCore* core, Instr* instr) {
    if (core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_Z_MASK)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jne_vn(CPUCore* core, Instr* instr) {
    if ((core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_Z_MASK) == 0)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jmi_vn(CPUCore* core, Instr* instr) {
    if (core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_N_MASK)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jpl_vn(CPUCore* core, Instr* instr) {
    if ((core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_N_MASK) == 0)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jvs_vn(CPUCore* core, Instr* instr) {
    if (core->sregs[HCAE_32V1_PST] &HCAE_32V1_PST_CONDF_V_MASK)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static void jvc_vn(CPUCore* core, Instr* instr) {
    if ((core->sregs[HCAE_32V1_PST] & HCAE_32V1_PST_CONDF_V_MASK) == 0)
        core->regs[HCAE_32V1_PC] += instr->val;
}


static inline void tst(CPUCore* core, HCAE_32V1Word a, HCAE_32V1Word b) {
    HCAE_32V1Word r = a & b;
    
    HCAE_32V1Word pst = core->sregs[HCAE_32V1_PST];

    pst &= ~(HCAE_32V1_PST_CONDF_N_MASK | HCAE_32V1_PST_CONDF_Z_MASK | HCAE_32V1_PST_CONDF_C_MASK | HCAE_32V1_PST_CONDF_V_MASK);
    
    pst |= (((r >> (HCAE_32V1_WORD_BITS - 1)) & 0x1) << HCAE_32V1_PST_CONDF_N_SHIFT);
    pst |= ((r == 0) << HCAE_32V1_PST_CONDF_Z_SHIFT);
    
    core->sregs[HCAE_32V1_PST] = pst;
}

static void tst_rr(CPUCore* core, Instr* instr) {
    tst(core, core->regs[instr->rd], core->regs[instr->rs]);
}

static void tst_rv(CPUCore* core, Instr* instr) {
    tst(core, core->regs[instr->rd], instr->val);
}


static void csr_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = cmp(core, core->regs[instr->rd], core->regs[instr->rs]);
} 

static void csr_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = cmp(core, core->regs[instr->rd], instr->val);
}


static inline HCAE_32V1Word adds(CPUCore* core, HCAE_32V1Word a, HCAE_32V1Word b) {
    HCAE_32V1Word r = a + b;
    
    HCAE_32V1Word pst = core->sregs[HCAE_32V1_PST];

    pst &= ~(HCAE_32V1_PST_CONDF_N_MASK | HCAE_32V1_PST_CONDF_Z_MASK | HCAE_32V1_PST_CONDF_C_MASK | HCAE_32V1_PST_CONDF_V_MASK);
    
    pst |= (((r >> (HCAE_32V1_WORD_BITS - 1)) & 0x1) << HCAE_32V1_PST_CONDF_N_SHIFT);
    pst |= ((r == 0) << HCAE_32V1_PST_CONDF_Z_SHIFT);
    pst |= ((r < a) << HCAE_32V1_PST_CONDF_C_SHIFT);
    pst |= (((~(a ^ b) & (a ^ r)) >> (HCAE_32V1_WORD_BITS - 1) & 0x1) << HCAE_32V1_PST_CONDF_V_SHIFT);
    
    core->sregs[HCAE_32V1_PST] = pst;
    return r;
}

static void adds_rr(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = adds(core, core->regs[instr->rd], core->regs[instr->rs]);
}

static void adds_rv(CPUCore* core, Instr* instr) {
    core->regs[instr->rd] = adds(core, core->regs[instr->rd], instr->val);
}


static void scall_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    
    cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_SC);
}


static void psr_rr(CPUCore* core, Instr* instr) {
    if (instr->rd >= _HCAE_32V1_SR_ENDOFLIST) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_UI);
        return;
    }
    
    core->sregs[instr->rd] = core->regs[instr->rs];
}



static void psrr_rr(CPUCore* core, Instr* instr) {
    if (instr->rs >= _HCAE_32V1_SR_ENDOFLIST) {
        core->regs[HCAE_32V1_PC] = instr->addr;
        cpu_core_syncexcept(core, HCAE_32V1_SYNCEXCEPT_UI);
        return;
    }
    
    core->regs[instr->rd] = core->sregs[instr->rs];
}


static void eret_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    core->regs[HCAE_32V1_PC] = core->sregs[HCAE_32V1_ILR];
}


instrtable instrs = {
    [HCAE_32V1_OPCODE_NOP] =       {[HCAE_32V1_OPERORDER_NN] = nop_nn},
    [HCAE_32V1_OPCODE_LD] =        {[HCAE_32V1_OPERORDER_RR] = ld_rr},
    [HCAE_32V1_OPCODE_ST] =        {[HCAE_32V1_OPERORDER_RR] = st_rr,     [HCAE_32V1_OPERORDER_RV] = st_rv},
    [HCAE_32V1_OPCODE_J] =         {[HCAE_32V1_OPERORDER_RN] = j_rn,      [HCAE_32V1_OPERORDER_VN] = j_vn},
    [HCAE_32V1_OPCODE_PUT] =       {[HCAE_32V1_OPERORDER_RR] = put_rr,    [HCAE_32V1_OPERORDER_RV] = put_rv},
    [HCAE_32V1_OPCODE_JL] =        {[HCAE_32V1_OPERORDER_RN] = jl_rn,     [HCAE_32V1_OPERORDER_VN] = jl_vn},
    [HCAE_32V1_OPCODE_RET] =       {[HCAE_32V1_OPERORDER_NN] = ret_nn,    [HCAE_32V1_OPERORDER_RN] = ret_rn},
    [HCAE_32V1_OPCODE_ADD] =       {[HCAE_32V1_OPERORDER_RR] = add_rr,    [HCAE_32V1_OPERORDER_RV] = add_rv},
    [HCAE_32V1_OPCODE_SUB] =       {[HCAE_32V1_OPERORDER_RR] = sub_rr,    [HCAE_32V1_OPERORDER_RV] = sub_rv},
    [HCAE_32V1_OPCODE_MUL] =       {[HCAE_32V1_OPERORDER_RR] = mul_rr,    [HCAE_32V1_OPERORDER_RV] = mul_rv},
    [HCAE_32V1_OPCODE_DIV] =       {[HCAE_32V1_OPERORDER_RR] = div_rr,    [HCAE_32V1_OPERORDER_RV] = div_rv},
    [HCAE_32V1_OPCODE_SDIV] =      {[HCAE_32V1_OPERORDER_RR] = sdiv_rr,   [HCAE_32V1_OPERORDER_RV] = sdiv_rv},
    [HCAE_32V1_OPCODE_CACT] =      {[HCAE_32V1_OPERORDER_NN] = cact_nn},
    [HCAE_32V1_OPCODE_SCRACT] =    {[HCAE_32V1_OPERORDER_RN] = scract_rn},
    [HCAE_32V1_OPCODE_SCRPC] =     {[HCAE_32V1_OPERORDER_RR] = scrpc_rr},
    [HCAE_32V1_OPCODE_AND] =       {[HCAE_32V1_OPERORDER_RR] = and_rr,    [HCAE_32V1_OPERORDER_RV] = and_rv},
    [HCAE_32V1_OPCODE_OR] =        {[HCAE_32V1_OPERORDER_RR] = or_rr,     [HCAE_32V1_OPERORDER_RV] = or_rv},
    [HCAE_32V1_OPCODE_XOR] =       {[HCAE_32V1_OPERORDER_RR] = xor_rr,    [HCAE_32V1_OPERORDER_RV] = xor_rv},
    [HCAE_32V1_OPCODE_NOT] =       {[HCAE_32V1_OPERORDER_RN] = not_rn},
    [HCAE_32V1_OPCODE_LSL] =       {[HCAE_32V1_OPERORDER_RR] = lsl_rr,    [HCAE_32V1_OPERORDER_RV] = lsl_rv},
    [HCAE_32V1_OPCODE_LSR] =       {[HCAE_32V1_OPERORDER_RR] = lsr_rr,    [HCAE_32V1_OPERORDER_RV] = lsr_rv},
    [HCAE_32V1_OPCODE_ASR] =       {[HCAE_32V1_OPERORDER_RR] = asr_rr,    [HCAE_32V1_OPERORDER_RV] = asr_rv},
    [HCAE_32V1_OPCODE_ROTR] =      {[HCAE_32V1_OPERORDER_RR] = rotr_rr,   [HCAE_32V1_OPERORDER_RV] = rotr_rv},
    [HCAE_32V1_OPCODE_PUSH] =      {[HCAE_32V1_OPERORDER_RN] = push_rn,   [HCAE_32V1_OPERORDER_VN] = push_vn},
    [HCAE_32V1_OPCODE_POP] =       {[HCAE_32V1_OPERORDER_NN] = pop_nn,    [HCAE_32V1_OPERORDER_RN] = pop_rn},
    [HCAE_32V1_OPCODE_SIRQM] =     {[HCAE_32V1_OPERORDER_RN] = sirqm_rn,  [HCAE_32V1_OPERORDER_VN] = sirqm_vn},
    [HCAE_32V1_OPCODE_CMP] =       {[HCAE_32V1_OPERORDER_RR] = cmp_rr,    [HCAE_32V1_OPERORDER_RV] = cmp_rv},
    [HCAE_32V1_OPCODE_JLO] =       {[HCAE_32V1_OPERORDER_VN] = jlo_vn},
    [HCAE_32V1_OPCODE_JHS] =       {[HCAE_32V1_OPERORDER_VN] = jhs_vn},
    [HCAE_32V1_OPCODE_JHI] =       {[HCAE_32V1_OPERORDER_VN] = jhi_vn},
    [HCAE_32V1_OPCODE_JLS] =       {[HCAE_32V1_OPERORDER_VN] = jls_vn},
    [HCAE_32V1_OPCODE_JLT] =       {[HCAE_32V1_OPERORDER_VN] = jlt_vn},
    [HCAE_32V1_OPCODE_JGE] =       {[HCAE_32V1_OPERORDER_VN] = jge_vn},
    [HCAE_32V1_OPCODE_JGT] =       {[HCAE_32V1_OPERORDER_VN] = jgt_vn},
    [HCAE_32V1_OPCODE_JLE] =       {[HCAE_32V1_OPERORDER_VN] = jle_vn},
    [HCAE_32V1_OPCODE_JEQ] =       {[HCAE_32V1_OPERORDER_VN] = jeq_vn},
    [HCAE_32V1_OPCODE_JNE] =       {[HCAE_32V1_OPERORDER_VN] = jne_vn},
    [HCAE_32V1_OPCODE_JMI] =       {[HCAE_32V1_OPERORDER_VN] = jmi_vn},
    [HCAE_32V1_OPCODE_JPL] =       {[HCAE_32V1_OPERORDER_VN] = jpl_vn},
    [HCAE_32V1_OPCODE_JVS] =       {[HCAE_32V1_OPERORDER_VN] = jvs_vn},
    [HCAE_32V1_OPCODE_JVC] =       {[HCAE_32V1_OPERORDER_VN] = jvc_vn},
    [HCAE_32V1_OPCODE_TST] =       {[HCAE_32V1_OPERORDER_RR] = tst_rr,     [HCAE_32V1_OPERORDER_RV] = tst_rv},
    [HCAE_32V1_OPCODE_CSR] =       {[HCAE_32V1_OPERORDER_RR] = csr_rr,     [HCAE_32V1_OPERORDER_RV] = csr_rv},
    [HCAE_32V1_OPCODE_ADDS] =      {[HCAE_32V1_OPERORDER_RR] = adds_rr,    [HCAE_32V1_OPERORDER_RV] = adds_rv},
    [HCAE_32V1_OPCODE_SCALL] =     {[HCAE_32V1_OPERORDER_NN] = scall_nn},
    [HCAE_32V1_OPCODE_PSR] =       {[HCAE_32V1_OPERORDER_RR] = psr_rr},
    [HCAE_32V1_OPCODE_PSRR] =      {[HCAE_32V1_OPERORDER_RR] = psrr_rr},
    [HCAE_32V1_OPCODE_ERET] =      {[HCAE_32V1_OPERORDER_NN] = eret_nn}
};
