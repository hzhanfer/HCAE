#include <stdio.h>
#include <stdint.h>
#include "instrs.h"
#include "opcodes.h"
#include "cpu.h"
#include "cpu-run.h"


static inline void set_flags_based_res(CPUCore*, HCAE_CPUWord, HCAE_CPUWord, HCAE_CPUWord);


static void nop_nn(CPUCore* core, Instr* instr) {
    (void)core;
    (void)instr;
    
    printf("nop\n");
}


static void ld_rr(CPUCore* core, Instr* instr) {
    printf("ld.%d\tr%d, [r%d]\n", instr->val_len, instr->rd, instr->rs);
    
    HCAE_CPUWord w = 0;
    
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


static void j_rn(CPUCore* core, Instr* instr) {
    printf("j\tr%d\n", instr->rs);
    
    core->regs[HCAE_REG_PC] = core->regs[instr->rs];
}

static void j_vn(CPUCore* core, Instr* instr) {
    printf("j\t#0x%x\n", instr->val);
    
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


static void jl_rn(CPUCore* core, Instr* instr) {
    printf("jl\tr%d\n", instr->rs);
    
    core->regs[HCAE_REG_LR] = core->regs[HCAE_REG_PC];
    core->regs[HCAE_REG_PC] = core->regs[instr->rs];
}

static void jl_vn(CPUCore* core, Instr* instr) {
    printf("jl\t#0x%x\n", instr->val);
    
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
    
    HCAE_CPUSWord r = core->regs[instr->rd] / core->regs[instr->rs];
    
    core->regs[instr->rd] = (HCAE_CPUWord)r;
}

static void sdiv_rv(CPUCore* core, Instr* instr) {
    printf("sdiv\tr%d, #0x%x\n", instr->rd, instr->val);
    
    if (instr->val == 0) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_DBZ);
        return;
    }
    
    HCAE_CPUSWord r = core->regs[instr->rd] / instr->val;
    
    core->regs[instr->rd] = r;
}


static void cact_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    
    printf("cact\n");
    
    core->sregs[HCAE_SREG_PST] &= ~PST_ACT_MASK;
}


static void scract_rn(CPUCore* core, Instr* instr) {
    printf("scract\tr%d\n", instr->rs);
    
    HCAE_CPUWord w = core->regs[instr->rs];
    
    uint8_t core_id = w & 0xf; // get bits 0-3
    if (core_id >= core->cpu->core_cnt) return; // ignore invalid core id
    
    uint8_t act = (w >> 4) & 0x1; // get bit 4
    
    core->sregs[HCAE_SREG_PST] = (core->regs[HCAE_SREG_PST] & ~PST_ACT_MASK) | (act << PST_ACT_SHIFT);
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


static inline HCAE_CPUWord asr(HCAE_CPUSWord d, HCAE_CPUSWord n) {
    if (n == 0) return d;
    if (n >= 32) n = 31;
    
    HCAE_CPUWord r = d >> n;
    
    if (r & 0x80000000)
        r |= ~(HCAE_CPU_WORD_MAX >> n);
    
    return r;
}

static void asr_rr(CPUCore* core, Instr* instr) {
    printf("asr\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] = asr(core->regs[instr->rd], (HCAE_CPUSWord)core->regs[instr->rs]);
}

static void asr_rv(CPUCore* core, Instr* instr) {
    printf("asr.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] = asr(core->regs[instr->rd], (HCAE_CPUSWord)instr->val);
}


static inline HCAE_CPUWord rotr(HCAE_CPUWord w, HCAE_CPUWord n) {
    n &= HCAE_CPU_WORD_BITS - 1;
    
    return (w >> n) | (w << ((HCAE_CPU_WORD_BITS - n) & (HCAE_CPU_WORD_BITS - 1)));
}

static void rotr_rr(CPUCore* core, Instr* instr) {
    printf("rotr\tr%d, r%d\n", instr->rs, instr->rd);
    
    core->regs[instr->rd] = rotr(core->regs[instr->rd], core->regs[instr->rs]);
}

static void rotr_rv(CPUCore* core, Instr* instr) {
    printf("rotr.%d\tr%d, #0x%x\n", instr->val_len, instr->rs, instr->val);
    
    core->regs[instr->rd] = rotr(core->regs[instr->rd], instr->val);
}


static void inline push(CPUCore* core, HCAE_CPUWord addr, HCAE_CPUWord w) {
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
    
    core->regs[HCAE_REG_SP] += sizeof(HCAE_CPUWord);
}

static void pop_rn(CPUCore* core, Instr* instr) {
    printf("pop\tr%d\n", instr->rd);
    
    if (!mem_read_arr(core->mem,
                      core->regs[HCAE_REG_SP],
                      (uint8_t*)&core->regs[instr->rd],
                      sizeof(HCAE_CPUWord))
    ) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_MF); 
        return;
    }
    
    core->regs[HCAE_REG_SP] += sizeof(HCAE_CPUWord);
}


static void sirqm_rn(CPUCore* core, Instr* instr) {
    printf("sirqm\tr%d\n", instr->rs);
    
    core->sregs[HCAE_SREG_PST] = (core->sregs[HCAE_SREG_PST] & ~PST_IRQM_MASK) | ((core->regs[instr->rs] & 0x1) << PST_IRQM_SHIFT);
}

static void sirqm_vn(CPUCore* core, Instr* instr) {
    printf("sirqm.%d\t#0x%x\n", instr->val_len, instr->val);
    
    core->sregs[HCAE_SREG_PST] = (core->sregs[HCAE_SREG_PST] & ~PST_IRQM_MASK) | ((instr->val & 0x1) << PST_IRQM_SHIFT);
}


static inline HCAE_CPUWord cmp(CPUCore* core, HCAE_CPUWord a, HCAE_CPUWord b) {
    HCAE_CPUWord r = a - b;
    
    HCAE_CPUWord pst = core->sregs[HCAE_SREG_PST];

    pst &= ~(PST_CMP_N_MASK | PST_CMP_Z_MASK | PST_CMP_C_MASK | PST_CMP_V_MASK);
    
    pst |= (((r >> (HCAE_CPU_WORD_BITS - 1)) & 0x1) << PST_CMP_N_SHIFT);
    pst |= ((r == 0) << PST_CMP_Z_SHIFT);
    pst |= ((a >= b) << PST_CMP_C_SHIFT);
    pst |= ((((a ^ b) & (a ^ r)) >> (HCAE_CPU_WORD_BITS - 1) & 0x1) << PST_CMP_V_SHIFT);
    
    core->sregs[HCAE_SREG_PST] = pst;
    
    return r;
}

static void cmp_rr(CPUCore* core, Instr* instr) {
    printf("cmp\tr%d, r%d\n", instr->rd, instr->rs);
    
    cmp(core, core->regs[instr->rd], core->regs[instr->rs]);
}

static void cmp_rv(CPUCore* core, Instr* instr) {
    printf("cmp.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    cmp(core, core->regs[instr->rd], instr->val);
}


static void jlo_vn(CPUCore* core, Instr* instr) {
    printf("j.lo.%d\t#0x%x\n", instr->val_len, instr->val);
    
    if ((core->sregs[HCAE_SREG_PST] & PST_CMP_C_MASK) == 0)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jhs_vn(CPUCore* core, Instr* instr) {
    printf("j.hs.%d\t#0x%x\n", instr->val_len, instr->val);
    
    if (core->sregs[HCAE_SREG_PST] & PST_CMP_C_MASK)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jhi_vn(CPUCore* core, Instr* instr) {
    printf("j.hi.%d\t#0x%x\n", instr->val_len, instr->val);
    
    uint8_t
        c = core->sregs[HCAE_SREG_PST] & PST_CMP_C_MASK,
        z = core->sregs[HCAE_SREG_PST] & PST_CMP_Z_MASK
    ;
    
    if (c == 1 && z == 0)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jls_vn(CPUCore* core, Instr* instr) {
    printf("j.ls.%d\t#0x%x\n", instr->val_len, instr->val);
    
    uint8_t
        c = core->sregs[HCAE_SREG_PST] & PST_CMP_C_MASK,
        z = core->sregs[HCAE_SREG_PST] & PST_CMP_Z_MASK
    ;
    
    if (c == 0 || z == 1)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jlt_vn(CPUCore* core, Instr* instr) {
    printf("j.lt.%d\t#0x%x\n", instr->val_len, instr->val);
    
    uint8_t
        n = core->sregs[HCAE_SREG_PST] & PST_CMP_N_MASK,
        v = core->sregs[HCAE_SREG_PST] & PST_CMP_V_MASK
    ;
    
    if (n != v)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jge_vn(CPUCore* core, Instr* instr) {
    printf("j.ge.%d\t#0x%x\n", instr->val_len, instr->val);
    
    uint8_t
        n = core->sregs[HCAE_SREG_PST] & PST_CMP_N_MASK,
        v = core->sregs[HCAE_SREG_PST] & PST_CMP_V_MASK
    ;
    
    if (n == v)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jgt_vn(CPUCore* core, Instr* instr) {
    printf("j.gt.%d\t#0x%x\n", instr->val_len, instr->val);
    
    uint8_t
        n = core->sregs[HCAE_SREG_PST] & PST_CMP_N_MASK,
        v = core->sregs[HCAE_SREG_PST] & PST_CMP_V_MASK,
        z = core->sregs[HCAE_SREG_PST] & PST_CMP_Z_MASK
    ;
    
    if (z == 0 && n == v)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jle_vn(CPUCore* core, Instr* instr) {
    printf("j.le.%d\t#0x%x\n", instr->val_len, instr->val);
    
    uint8_t
        n = core->sregs[HCAE_SREG_PST] & PST_CMP_N_MASK,
        v = core->sregs[HCAE_SREG_PST] & PST_CMP_V_MASK,
        z = core->sregs[HCAE_SREG_PST] & PST_CMP_Z_MASK
    ;
    
    if (z == 1 && n != v)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jeq_vn(CPUCore* core, Instr* instr) {
    printf("j.eq.%d\t#0x%x\n", instr->val_len, instr->val);
    
    if (core->sregs[HCAE_SREG_PST] & PST_CMP_Z_MASK)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jne_vn(CPUCore* core, Instr* instr) {
    printf("j.ne.%d\t#0x%x\n", instr->val_len, instr->val);
    
    if ((core->sregs[HCAE_SREG_PST] & PST_CMP_Z_MASK) == 0)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jmi_vn(CPUCore* core, Instr* instr) {
    printf("j.mi.%d\t#0x%x\n", instr->val_len, instr->val);
    
    if (core->sregs[HCAE_SREG_PST] & PST_CMP_N_MASK)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jpl_vn(CPUCore* core, Instr* instr) {
    printf("j.pl.%d\t#0x%x\n", instr->val_len, instr->val);
    
    if ((core->sregs[HCAE_SREG_PST] & PST_CMP_N_MASK) == 0)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jvs_vn(CPUCore* core, Instr* instr) {
    printf("j.vs.%d\t#0x%x\n", instr->val_len, instr->val);
    
    if (core->sregs[HCAE_SREG_PST] & PST_CMP_V_MASK)
        core->regs[HCAE_REG_PC] += instr->val;
}


static void jvc_vn(CPUCore* core, Instr* instr) {
    printf("j.vc.%d\t#0x%x\n", instr->val_len, instr->val);
    
    if ((core->sregs[HCAE_SREG_PST] & PST_CMP_V_MASK) == 0)
        core->regs[HCAE_REG_PC] += instr->val;
}


static inline void tst(CPUCore* core, HCAE_CPUWord a, HCAE_CPUWord b) {
    HCAE_CPUWord r = a & b;
    
    HCAE_CPUWord pst = core->sregs[HCAE_SREG_PST];

    pst &= ~(PST_CMP_N_MASK | PST_CMP_Z_MASK | PST_CMP_C_MASK | PST_CMP_V_MASK);
    
    pst |= (((r >> (HCAE_CPU_WORD_BITS - 1)) & 0x1) << PST_CMP_N_SHIFT);
    pst |= ((r == 0) << PST_CMP_Z_SHIFT);
    
    core->sregs[HCAE_SREG_PST] = pst;
}

static void tst_rr(CPUCore* core, Instr* instr) {
    printf("tst\tr%d, r%d\n", instr->rd, instr->rs);
    
    tst(core, core->regs[instr->rd], core->regs[instr->rs]);
}

static void tst_rv(CPUCore* core, Instr* instr) {
    printf("tst.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    tst(core, core->regs[instr->rd], instr->val);
}


static void csr_rr(CPUCore* core, Instr* instr) {
    printf("csr\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] = cmp(core, core->regs[instr->rd], core->regs[instr->rs]);
} 

static void csr_rv(CPUCore* core, Instr* instr) {
    printf("csr.%d\tr%d, #0x%x\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] = cmp(core, core->regs[instr->rd], instr->val);
}


static inline HCAE_CPUWord adds(CPUCore* core, HCAE_CPUWord a, HCAE_CPUWord b) {
    HCAE_CPUWord r = a + b;
    
    HCAE_CPUWord pst = core->sregs[HCAE_SREG_PST];

    pst &= ~(PST_CMP_N_MASK | PST_CMP_Z_MASK | PST_CMP_C_MASK | PST_CMP_V_MASK);
    
    pst |= (((r >> (HCAE_CPU_WORD_BITS - 1)) & 0x1) << PST_CMP_N_SHIFT);
    pst |= ((r == 0) << PST_CMP_Z_SHIFT);
    pst |= ((r < a) << PST_CMP_C_SHIFT);
    pst |= (((~(a ^ b) & (a ^ r)) >> (HCAE_CPU_WORD_BITS - 1) & 0x1) << PST_CMP_V_SHIFT);
    
    core->sregs[HCAE_SREG_PST] = pst;
    return r;
}

static void adds_rr(CPUCore* core, Instr* instr) {
    printf("adds\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] = adds(core, core->regs[instr->rd], core->regs[instr->rs]);
}

static void adds_rv(CPUCore* core, Instr* instr) {
    printf("adds.%d\tr%d, r%d\n", instr->val_len, instr->rd, instr->val);
    
    core->regs[instr->rd] = adds(core, core->regs[instr->rd], instr->val);
}


static void scall_nn(CPUCore* core, Instr* instr) {
    (void)instr;
    
    printf("scall\n");
    
    cpu_core_syncexcept(core, SYNCEXCEPT_SC);
}


static void psr_rr(CPUCore* core, Instr* instr) {
    if (instr->rd >= CPU_NUM_SREGS) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_UI);
        return;
    }
    
    printf("psr\tsr%d, r%d\n", instr->rd, instr->rs);
    
    core->sregs[instr->rd] = core->regs[instr->rs];
}



static void psrr_rr(CPUCore* core, Instr* instr) {
    if (instr->rs >= CPU_NUM_SREGS) {
        core->regs[HCAE_REG_PC] = instr->addr;
        cpu_core_syncexcept(core, SYNCEXCEPT_UI);
        return;
    }
    
    printf("psrr\tr%d, r%d\n", instr->rd, instr->rs);
    
    core->regs[instr->rd] = core->sregs[instr->rs];
}


static void eret_nn(CPUCore* core, Instr* instr) {
    printf("eret\n");
    
    core->regs[HCAE_REG_PC] = core->sregs[HCAE_SREG_ELR];
}


instrtable instrs = {
    [OPCODE_NOP] = {[OPERORDER_NN] = nop_nn},
    [OPCODE_LD] = {[OPERORDER_RR] = ld_rr},
    [OPCODE_ST] = {[OPERORDER_RR] = st_rr, [OPERORDER_RV] = st_rv},
    [OPCODE_J] = {[OPERORDER_RN] = j_rn, [OPERORDER_VN] = j_vn},
    [OPCODE_PUT] = {[OPERORDER_RR] = put_rr, [OPERORDER_RV] = put_rv},
    [OPCODE_JL] = {[OPERORDER_RN] = jl_rn, [OPERORDER_VN] = jl_vn},
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
    [OPCODE_CMP] = {[OPERORDER_RR] = cmp_rr, [OPERORDER_RV] = cmp_rv},
    [OPCODE_JLO] = {[OPERORDER_VN] = jlo_vn},
    [OPCODE_JHS] = {[OPERORDER_VN] = jhs_vn},
    [OPCODE_JHI] = {[OPERORDER_VN] = jhi_vn},
    [OPCODE_JLS] = {[OPERORDER_VN] = jls_vn},
    [OPCODE_JLT] = {[OPERORDER_VN] = jlt_vn},
    [OPCODE_JGE] = {[OPERORDER_VN] = jge_vn},
    [OPCODE_JGT] = {[OPERORDER_VN] = jgt_vn},
    [OPCODE_JLE] = {[OPERORDER_VN] = jle_vn},
    [OPCODE_JEQ] = {[OPERORDER_VN] = jeq_vn},
    [OPCODE_JNE] = {[OPERORDER_VN] = jne_vn},
    [OPCODE_JMI] = {[OPERORDER_VN] = jmi_vn},
    [OPCODE_JPL] = {[OPERORDER_VN] = jpl_vn},
    [OPCODE_JVS] = {[OPERORDER_VN] = jvs_vn},
    [OPCODE_JVC] = {[OPERORDER_VN] = jvc_vn},
    [OPCODE_TST] = {[OPERORDER_RR] = tst_rr, [OPERORDER_RV] = test_rv},
    [OPCODE_CSR] = {[OPERORDER_RR] = csr_rr, [OPERORDER_RV] = csr_rv},
    [OPCODE_ADDS] = {[OPERORDER_RR] = adds_rr, [OPERORDER_RV] = adds_rv},
    [OPCODE_SCALL] = {[OPERORDER_NN] = scall_nn}
    [OPCODE_PSR] = {[OPERORDER_RR] = psr_rr},
    [OPCODE_PSRR] = {[OPERORDER_RR] = psrr_rr},
    [OPCODE_ERET] = {[OPERORDER_NN] = eret_nn}
};
