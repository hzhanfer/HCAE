#ifndef OPCODES_H
#define OPCODES_H


enum {
    _OPCODE_UDF = 0,
    OPCODE_NOP, // no operation
    OPCODE_LD, // memory load
    OPCODE_ST, // memory store
    OPCODE_JMP, // unconditional jump
    OPCODE_PUT, // put
    OPCODE_JMPL, // unconditional jump with link
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
    _OPCODE_ENDOFLIST
};


#endif