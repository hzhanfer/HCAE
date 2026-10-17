# Instruction Format

Variable Length (2–7 bytes)

1. First Byte

 - Bits 0–7: Opcode (lower 8 bits)

2. Second Byte

 - Bits 8–10: Opcode (upper 3 bits)
 - Bits 11–13: Operand Order (3 bits)
 - Bits 14-15: Value Length (2 bits)

3. Third Byte (Register Numbers)

This field only appears when Operand Order is RN, RR, or RV.

 - Bits 16-19: rDestination (rD)
 - Bits 20-24: rSource (rS) (reserved for RN and RV)

4. Third/Fourth to Sixth Bytes

 - Bits 16–47 or 22-53 for RN, RR and RV: Value (0–32 bits, depending on Value Length)

## Operand Order Values

1. NN (0): None, none. No operands.
2. RN (1): Register, none. One register.
3. RR (2): Register, register. Two registers.
4. RV (3): Register, value. One register, one value.
5. VN (4): Value, none. One Value.
