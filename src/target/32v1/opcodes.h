#ifndef OPCODES_H
#define OPCODES_H


enum {
    _OPCODE_UDF = 0,
    OPCODE_NOP, // no operation
    OPCODE_LD, // memory load
    OPCODE_ST, // memory store
    OPCODE_J, // unconditional jump
    OPCODE_PUT, // put value/register to register
    OPCODE_JL, // unconditional jump with link
    OPCODE_RET, // return
    OPCODE_ADD, // add
    OPCODE_SUB, // substract
    OPCODE_MUL, // multiply
    OPCODE_DIV, // division
    OPCODE_SDIV, // signed division
    OPCODE_CACT, // clear PST:ACT
    OPCODE_SCRACT, // set core PST:ACT
    OPCODE_SCRPC, // set core PC
    OPCODE_AND, // bitwise and
    OPCODE_OR, // bitwise or
    OPCODE_XOR, // bitwise xor
    OPCODE_NOT, // bitwise not
    OPCODE_LSL, // logical shift left
    OPCODE_LSR, // logical shift right
    OPCODE_ASR, // arithmetic shift right
    OPCODE_ROTR, // rotate right
    OPCODE_PUSH, // push
    OPCODE_POP, // pop
    OPCODE_SIRQM, // set IRQ mask
    OPCODE_CMP, // compare
    OPCODE_JLO, // C == 0             lower
    OPCODE_JHS, // C == 1             higher or same
    OPCODE_JHI, // C == 1 && Z == 0   higher
    OPCODE_JLS, // C == 0 || Z == 1   lower or same
    OPCODE_JLT, // N != V             less than
    OPCODE_JGE, // N == V             greater or equal
    OPCODE_JGT, // Z == 0 && N == V   greater than
    OPCODE_JLE, // Z == 1 || N != V   less or equal
    OPCODE_JEQ, // Z == 1
    OPCODE_JNE, // Z == 0
    OPCODE_JMI, // N == 1
    OPCODE_JPL, // N == 0
    OPCODE_JVS, // V == 1
    OPCODE_JVC, // V == 0
    OPCODE_TST, // test
    OPCODE_CSR, // compare and save result in rD
    OPCODE_ADDS, // add and set flags
    OPCODE_SCALL, // system call
    OPCODE_PSR, // put system register
    OPCODE_PSRR, // put system register to register
    OPCODE_ERET, // return from exception
    _OPCODE_ENDOFLIST
};


#endif