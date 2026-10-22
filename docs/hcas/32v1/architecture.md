# Himrich's CPU Architecture 32 Version 1 (hca32v1)

The first 32-bit HCA specification.

A simple architecture that covers most of the essential things a CPU needs, such as a full ISA, interrupt-handling, core execution management, arithmetics, bitwise/logical, memory mangement, etc..

The ISA can be found [here](isa.md).

## Registers

It has 13 general-purpose registers, 3 special-purpose(counts as GPRs) registers, and 3 system-registers:

 - r0-r12: GPRs
 - r13: Stack Pointer (SP)
 - r14: Link Register (LR)
 - r15: Program Counter (PC)
 - PST (sr0): Processor State, stores essential processor data, and comparison results.
 - IDAT (sr1): Interrupt Data, stores information describing the recent exception.
 - ILR (sr2): Interrupt Link Register, stores the address of the instruction associated with the exception.

### SP Rules

The Stack Pointer has no enforced rules such as alignment-rules, it must be valid and set by software.

### IDAT

The Interrupt Data register stores debug data after an interrupt occurs.

For Synchronous Exceptions its value only contains type of Synchronous Exception.

(DEV: NMI and IRQ handling is not yet defined)

### PST

PST holds flags and data that controls how the CPU behaves.

#### Bits and bit fields

 - PST:ACT (bit 0): ACT stands for Active, it tells whether a core is active or not.
 - PST:IRQM (bit 1): IRQM stands for IRQ Mask, it tells whether IRQs are masked or unmasked.
 - PST:CMP:N (bit 2): Condition Flags, Negative.
 - PST:CMP:Z (bit 3): Condition Flags, Zero.
 - PST:CMP:C (bit 4): Condition Flags, Carry.
 - PST:CMP:V (bit 5): Condition Flags, Overflow.

## Synchronous Exception Types

 - Memory Fault (MF, #0): Happens when the CPU accesses an invalid memory address(out-of-bounds), or an attempt to write in ROM was made.
 - Unknown Instruction (UI, #1)
 - Division by Zero (DBZ, #2)
 - System Call (SC, #3): Triggered by ``SCALL`` instruction.

## Vector Table

The vector table resides at address 0x00000000 in ROM. Each entry contains a 32-bit address.

 - 0x00000000: Reset vector
 - 0x00000004: Non-maskable interrupt (NMI) vector
 - 0x00000008: Interrupt request (IRQ) vector
 - 0x0000000C: Synchronous exception vector

## Exception Handling

When a synchronous exception occurs, the CPU saves the address of the faulting instruction in ILR, records the exception cause in IDAT, and transfers execution to the Synchronous Exception handler.
The exception handler may inspect IDAT and ILR to determine the cause of the exception and decide how to proceed.
