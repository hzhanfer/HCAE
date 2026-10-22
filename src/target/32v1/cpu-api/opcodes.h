#ifndef HCAE_32V1_OPCODES_H
#define HCAE_32V1_OPCODES_H


typedef enum HCAE_32V1Opcode {
    _HCAE_32V1_OPCODE_STARTOFLIST = 0,
    HCAE_32V1_OPCODE_NOP, // no operation
    HCAE_32V1_OPCODE_LD, // memory load
    HCAE_32V1_OPCODE_ST, // memory store
    HCAE_32V1_OPCODE_J, // unconditional jump
    HCAE_32V1_OPCODE_PUT, // put value/register to register
    HCAE_32V1_OPCODE_JL, // unconditional jump with link
    HCAE_32V1_OPCODE_RET, // return
    HCAE_32V1_OPCODE_ADD, // add
    HCAE_32V1_OPCODE_SUB, // substract
    HCAE_32V1_OPCODE_MUL, // multiply
    HCAE_32V1_OPCODE_DIV, // division
    HCAE_32V1_OPCODE_SDIV, // signed division
    HCAE_32V1_OPCODE_CACT, // clear PST:ACT
    HCAE_32V1_OPCODE_SCRACT, // set core PST:ACT
    HCAE_32V1_OPCODE_SCRPC, // set core PC
    HCAE_32V1_OPCODE_AND, // bitwise and
    HCAE_32V1_OPCODE_OR, // bitwise or
    HCAE_32V1_OPCODE_XOR, // bitwise xor
    HCAE_32V1_OPCODE_NOT, // bitwise not
    HCAE_32V1_OPCODE_LSL, // logical shift left
    HCAE_32V1_OPCODE_LSR, // logical shift right
    HCAE_32V1_OPCODE_ASR, // arithmetic shift right
    HCAE_32V1_OPCODE_ROTR, // rotate right
    HCAE_32V1_OPCODE_PUSH, // push
    HCAE_32V1_OPCODE_POP, // pop
    HCAE_32V1_OPCODE_SIRQM, // set IRQ mask
    HCAE_32V1_OPCODE_CMP, // compare
    HCAE_32V1_OPCODE_JLO, // C == 0             lower
    HCAE_32V1_OPCODE_JHS, // C == 1             higher or same
    HCAE_32V1_OPCODE_JHI, // C == 1 && Z == 0   higher
    HCAE_32V1_OPCODE_JLS, // C == 0 || Z == 1   lower or same
    HCAE_32V1_OPCODE_JLT, // N != V             less than
    HCAE_32V1_OPCODE_JGE, // N == V             greater or equal
    HCAE_32V1_OPCODE_JGT, // Z == 0 && N == V   greater than
    HCAE_32V1_OPCODE_JLE, // Z == 1 || N != V   less or equal
    HCAE_32V1_OPCODE_JEQ, // Z == 1
    HCAE_32V1_OPCODE_JNE, // Z == 0
    HCAE_32V1_OPCODE_JMI, // N == 1
    HCAE_32V1_OPCODE_JPL, // N == 0
    HCAE_32V1_OPCODE_JVS, // V == 1
    HCAE_32V1_OPCODE_JVC, // V == 0
    HCAE_32V1_OPCODE_TST, // test
    HCAE_32V1_OPCODE_CSR, // compare and save result in rD
    HCAE_32V1_OPCODE_ADDS, // add and set flags
    HCAE_32V1_OPCODE_SCALL, // system call
    HCAE_32V1_OPCODE_PSR, // put system register
    HCAE_32V1_OPCODE_PSRR, // put system register to register
    HCAE_32V1_OPCODE_ERET, // return from exception
    _HCAE_32V1_OPCODE_ENDOFLIST
} HCAE_32V1Opcode;


#endif