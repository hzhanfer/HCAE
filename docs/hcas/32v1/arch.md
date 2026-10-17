# Himrich's CPU Architecture 32 Version 1 (hca32v1)

The first 32-bit HCA specification.

## Registers

It has 10 general-purpose registers, 3 special-purpose registers, and 2 system-registers:

 - r0-r9: GPRs
 - r13: Stack Pointer (SP)
 - r14: Link Register (LR)
 - r15: Program Counter (PC)
 - PST (System-register): Processor State, stores essential processor data, and comparison results.
 - EDAT (System-register): Exception data, stores information describing the recent exception.
 - ELR: Exception Link Register, stores the address of the instruction associated with the exception.

## Vector Table

The vector table resides at address 0x00000000 in ROM. Each entry contains a 32-bit address.

 - 0x00000000: Reset vector
 - 0x00000004: Non-maskable interrupt (NMI) vector
 - 0x00000008: Interrupt request (IRQ) vector
 - 0x0000000C: Synchronous exception vector

## Exception Handling

When a synchronous exception occurs, the CPU saves the address of the faulting instruction in ELR, records the exception cause in EDAT, and transfers execution to the handler address stored at 0x0000000C.
The exception handler may inspect EDAT and ELR to determine the cause of the exception and decide how to proceed.

## Instruction Set Architecture

The ISA can be found [here](isa.md).