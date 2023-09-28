#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bof.h"
#include "instruction.h"
#include "regname.h"
#include "utilities.h"
#include "machine_types.h"

#ifndef HW1_TESTS_MACHINE_H
#define HW1_TESTS_MACHINE_H

#endif //HW1_TESTS_MACHINE_H

extern void setRegister(char *name, int data);


//region REGISTER FORMAT INSTRUCTION:

    //name| op| rs rt rd | shift | func | (Explanation)
    //ADD   0   s t d       -       33    Add: GPR[d] ← GPR[s] + GPR[t]
extern void ADD(bin_instr_t instruction);
    //SUB 0 s t d - 35 Subtract: GPR[d] ← GPR[s] − GPR[t]
extern void SUB(int s, int t, int d);
    //MUL 0 s t - - 25 (HI, LO) ← GPR[s] × GPR[t] Multiply: Multiply GPR[s] and GPR[t], putting the least significant bits in LO //and the most significant bits in HI.
extern void MUL(int s, int t);

    //DIV 0 s t - - 27 Divide (remainder in HI, quotient in LO): //HI ← GPR[s] % GPR[t] //LO ← GPR[s]/GPR[t])
extern void DIV(int s, int t);
    //MFHI 0 - - d - 16 Move from HI: GPR[d] ← HI
    //        MFLO 0 - - d - 18 Move from LO: GPR[d] ← LO
    //        AND 0 s t d - 36 Bitwise And: GPR[d] ← GPR[s] ∧ GPR[t]
    //BOR 0 s t d - 37 Bitwise Or: GPR[d] ← GPR[s] ∨ GPR[t]
    //NOR 0 s t d - 39 Bitwise Not-Or: GPR[d] ← ¬(GPR[s] ∨ GPR[t])
    //XOR 0 s t d - 38 Bitwise Exclusive-Or: GPR[d] ← GPR[s] xor GPR[t]
    //SLL 0 - t d h 0 Shift Left Logical: GPR[d] ← GPR[t] « h
    //        SRL 0 - t d h 3 Shift Right Logical: GPR[d] ← GPR[t] » h
    //        JR 0 s 0 0 0 8 Jump Register: PC ← GPR[s]
    //SYSCALL 0 - - - - 12 System Call: (see Table 6)

//endregion
//region IMMEDIATE FORMAT INSTRUCTION:

    //Name op rs rt immed (Explanation)
extern void ADDI(bin_instr_t intrsuction);
    //ADDI 9 s t i Add immediate: GPR[t] ← GPR[s] + sgnExt(i)
extern void ANDI(bin_instr_t intrsuction);
    //ANDI 12 s t i Bitwise And immediate: GPR[t] ← GPR[s] ∧ zeroExt(i)
extern void BORI(bin_instr_t intrsuction);
    //BORI 13 s t i Bitwise Or immediate: GPR[t] ← GPR[s] ∨ zeroExt(i)
extern void XORI(bin_instr_t intrsuction);
    //XORI 14 s t i Bitwise Xor immediate: GPR[t] ← GPR[s] xor zeroExt(i)
extern void BEQ(bin_instr_t intrsuction);
    //BEQ 4 s t o Branch on Equal: if GPR[s] = GPR[t] then PC ← PC + formOffset(o)
extern void BGEZ(bin_instr_t intrsuction);
    //BGEZ 1 s 1 o Branch ≥ 0: if GPR[s] ≥ 0 then PC ← PC + formOffset(o)
extern void BGTZ(bin_instr_t intrsuction);
    //BGTZ 7 s 0 o Branch > 0: if GPR[s] > 0 then PC ← PC + formOffset(o)
extern void BLEZ(bin_instr_t intrsuction);
    //BLEZ 6 s 0 o Branch ≤ 0: if GPR[s] ≤ 0 then PC ← PC + formOffset(o)
extern void BLTZ(bin_instr_t intrsuction);
    //BLTZ 8 s 0 o Branch < 0: if GPR[s] < 0 then PC ← PC + formOffset(o)
extern void BNE(bin_instr_t intrsuction);
    //BNE 5 s t o Branch Not Equal: if GPR[s] ̸ = GPR[t] then PC ← PC + formOffset(o)
extern void LBU(bin_instr_t intrsuction);
    //LBU 36 b t o Load Byte Unsigned:
    //GPR[t] ← zeroExt(memory[GPR[b] + formOffset(o)])
extern void LW(bin_instr_t intrsuction);
    //LW 35 b t o Load Word (4 bytes):
    //GPR[t] ← memory[GPR[b] + formOffset(o)]
extern void SB(bin_instr_t intrsuction);
    //SB 40 b t o Store Byte (least significant byte of GPR[t]):
    //memory[GPR[b] + formOffset(o)] ← GPR[t]
extern void SW(bin_instr_t intrsuction);
    //SW 43 b t o Store Word (4 bytes):
    //memory[GPR[b] + formOffset(o)] ← GPR[t]

//endregion
//region JUMP TYPE INSTRUCTIONS

    //Name op addr (Explanation)

    //JMP 2 a Jump: PC ← formAddress(P C, a)
    //JAL 3 a Jump and Link: GPR[$ra] ← PC; PC ← formAddress(PC, a)

    //endregion
    //region System Calls

    //code name arg. reg. Effect (in terms of C std. library)

    //10 EXIT - exit(0) //halt
    //4 PSTR $a0 GPR[$v0] ← printf("%s",&memory[GPR[$a0]])
    //11 PCH $a0 GPR[$v0] ←fputc(GPR[$a0],stdout)
    //12 RCH - GPR[$v0] ← getc(stdin)
    //256 STRA - start VM tracing; start tracing output
    //257 NOTR - no VM tracing; stop the tracing output

//endregion
