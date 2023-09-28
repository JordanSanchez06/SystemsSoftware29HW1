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
extern int PC;


//region REGISTER FORMAT INSTRUCTION:

    //name| op| rs rt rd | shift | func | (Explanation)
    //ADD   0   s t d       -       33    Add: GPR[d] ← GPR[s] + GPR[t]
extern void ADD(bin_instr_t instruction);
    //SUB 0 s t d - 35 Subtract: GPR[d] ← GPR[s] − GPR[t]
extern void SUB(bin_instr_t instruction);
    //MUL 0 s t - - 25 (HI, LO) ← GPR[s] × GPR[t] Multiply: Multiply GPR[s] and GPR[t], putting the least significant bits in LO //and the most significant bits in HI.
extern void MUL(bin_instr_t instruction);

    //DIV 0 s t - - 27 Divide (remainder in HI, quotient in LO): //HI ← GPR[s] % GPR[t] //LO ← GPR[s]/GPR[t])
extern void DIV(bin_instr_t instruction);
//MFHI 0 - - d - 16 Move from HI: GPR[d] ← HI
void MFHI(bin_instr_t instruction);
//        MFLO 0 - - d - 18 Move from LO: GPR[d] ← LO
void MFLO(bin_instr_t instruction);
//        AND 0 s t d - 36 Bitwise And: GPR[d] ← GPR[s] ∧ GPR[t]
void AND(bin_instr_t instruction);
//BOR 0 s t d - 37 Bitwise Or: GPR[d] ← GPR[s] ∨ GPR[t]
void BOR(bin_instr_t instruction);
//NOR 0 s t d - 39 Bitwise Not-Or: GPR[d] ← ¬(GPR[s] ∨ GPR[t])
void NOR(bin_instr_t instruction);
//XOR 0 s t d - 38 Bitwise Exclusive-Or: GPR[d] ← GPR[s] xor GPR[t]
void XOR(bin_instr_t instruction);
//SLL 0 - t d h 0 Shift Left Logical: GPR[d] ← GPR[t] « h
void SLL(bin_instr_t instruction);
//SRL 0 - t d h 3 Shift Right Logical: GPR[d] ← GPR[t] » h
void SRL(bin_instr_t instruction);
//JR 0 s 0 0 0 8 Jump Register: PC ← GPR[s]
void JR(bin_instr_t instruction);

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
