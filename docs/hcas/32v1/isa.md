# Instruction Format

Variable Length (2–7 bytes)

1. First Byte

 - Bits 0–7: Opcode (lower 8 bits)

2. Second Byte

 - Bits 8–10: Opcode (upper 3 bits)
 - Bits 11–13: Operand Order (3 bits)
 - Bits 14-15: Value Length (2 bits)

3. Third Byte (Register Numbers)

This field only appears when the Operand Order is RN, RR, or RV.

 - Bits 16-19: rDestination (rD)
 - Bits 20-24: rSource (rS) (ignored for RN and RV)

4. Third/Fourth to Sixth Bytes

This field only appears when the Operand Order is VN, or RV.

 - Bits 16–47 for when the Operand Order is VN, or bits 22-53 for when the Operand Order is RV: Value (0–32 bits, depending on Value Length)

## Operand Order Values

1. NN (0): None, none. No operands.
2. RN (1): Register, none. One register.
3. RR (2): Register, register. Two registers.
4. RV (3): Register, value. One register, one value.
5. VN (4): Value, none. One Value.

## Decoding

 - The CPU first fetches two bytes, those two bytes serve as the Header.
 - The CPU then combines bits 0-7 of byte 1 and bits 8-10 of byte 2, forming the Opcode.
 - Then it checks if the Opcode is 0x0 or is unknown. If it is 0x0 or unknown it triggers an Unknown Instruction.
 - It then decodes bits 11-13 of byte 2 to form the Operand Order.
 - Checks if the Operand Order is greater or equal to 0x5, if it is, it triggers an Unknown Instruction.
 - If the Operand Order is RN, RR, or RV then it fetches another byte. It then decodes that byte as rD(lower 4-bits) and rS(upper 4-bits).
 - Decodes bits 14-15 of byte 2 and adds 1 to form the Value Length.
 - If the Operand Order is RV or VN, then it fetches bytes based on Value Length. Then it combines those bytes to form the Value which is zeroed before being set.
 - Then it runs the instruction.

## Instructions

1. 0b00000000000, ``nop``: **N**o **Op**eration.

``nop``

Does Nothing.

2. 0b00000000001, ``ld.(n) rD, [rS]``: **L**oa**d** from Memory.

``ld.(n) rD, [rS]``

Loads (n) byte(s) from the memory address stored in [rS] into rD.

3. 0b00000000010, ``st.(n) [rD], rS/#imm``: **St**ore in Memory.

``st.(n) [rD], rS``
``st.(n) [rD], #imm``

Stores (n) byte(s) from rS/#imm into the memory address stored in [rD].

(n) serves as the amount of bytes to store and as the length of the immediate.

4. 0b00000000011, ``j rS/#addr``: Unconditional **J**ump.

Performs an unconditional jump.

``j rS``
``j #addr``

For ``j rS`` it performs an absolute jump into the memory address stored in rS.

For ``j #addr`` it performs a PC-relative jump to #addr.

5. 0b00000000100, ``put.(n) rD, rS/#imm``: **Put** a Value into a Register.

``put rD, rS``
``put.(n) rD, #imm``

For ``put rD, rS`` it copies rS into rD.

For ``put.(n) rD, #imm`` zeroes rD and stores an (n) byte-long immediate in rD.

6. 0b00000000101, ``jl rS/#addr``: Unconditional **J**ump with **L**ink.

``jl rS``
``jl #addr``

For ``jl rS`` it performs an absolute jump into the memory address stored in rS.

For ``jl #addr`` it performs a PC-relative jump to #addr.

Both copy the PC into the LR before the jump.

7. 0b00000000110, ``ret rS``: **Ret**urn.

``ret``
``ret rS``

For ``ret`` it copies the LR into the PC.

For ``ret rS`` it copies rS into the PC.

8. 0b00000000111, ``add.(n) rD, rS/#imm``: **Add** and Store

Signed/unsigned.

``add rD, rS``
``add.(n) rD, #imm``

Adds rD and rS/#imm then stores the result in rD.

9. 0b00000001000, ``sub.(n) rD, rS/#imm``: **Sub*stract and Store.

Signed/unsigned.

``sub rD, rS``
``sub.(n) rD, #imm``

Substracts rD and rS/#imm then stores the result in rD.

10. 0b00000001001, ``mul.(n) rD, rS/#imm``: Multiply and Store.

Signed/unsigned.

``mul rD, rs``
``mul.(n) rD, #imm``

Multiplies rD and rS/#imm then stores the result in rD.

11. 0b00000001010, ``div.(n) rD, rS/#imm``: Divide and Store.

Unsigned.

``div rD, rS``
``div.(n) rD, #imm``

Divides rD and rS/#imm then stores the result in rD.

12. 0b00000001011, ``sdiv.(n) rD, rS/#imm``: Divide and Store.

Signed.

``sdiv rD, rS``
``sdiv.(n) rD, rS``

Divides rD and rS/#imm then stores the result in rD.

13. 0b00000001100, ``cact``: **C**lear the PST:**ACT** of the Current Core.

``cact``

Sets the PST:ACT of the current core to 0.

14. 0b00000001101, ``scract rS``: