#include "machine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "regname.h"
#include "utilities.h"
#include "machine_types.h"
#include "machine_main.h"

extern int REGISTERS[NUM_REGISTERS];
address_type HI;
address_type LO;

void setRegister(char *name, int data){
    int ptr = 0;
    while(ptr < NUM_REGISTERS){
        if(strcmp(name, regname_get(ptr)) == 0){
            REGISTERS[ptr] = data;
            return;
        }
        ptr++;
    }
    bail_with_error("register does not exist");
}

int getRegister(char *name){
    int ptr = 0;
    while(ptr < NUM_REGISTERS){
        if(strcmp(name, regname_get(ptr)) == 0){
            return REGISTERS[ptr];
        }
        ptr++;
    }
    bail_with_error("register does not exist");
    return 0;
}

//region REGISTER FORMAT INSTRUCTION:
    //name| op| rs rt rd | shift | func | (Explanation)
    //ADD   0   s t d       -       33    Add: GPR[d] ← GPR[s] + GPR[t]
void ADD(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd] = REGISTERS[instruction.reg.rs] + REGISTERS[instruction.reg.rt];
}
    //SUB 0 s t d - 35 Subtract: GPR[d] ← GPR[s] − GPR[t]
void SUB(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd] = REGISTERS[instruction.reg.rs] - REGISTERS[instruction.reg.rt];
}
    //MUL 0 s t - - 25 (HI, LO) ← GPR[s] × GPR[t] Multiply: Multiply GPR[s] and GPR[t], putting the least significant bits in LO //and the most significant bits in HI.
void MUL(bin_instr_t instruction){
    LO = REGISTERS[instruction.reg.rs] * REGISTERS[instruction.reg.rt];
    HI = ((long long) REGISTERS[instruction.reg.rs] * REGISTERS[instruction.reg.rt]) >> 32;
}

    //DIV 0 s t - - 27 Divide (remainder in HI, quotient in LO): //HI ← GPR[s] % GPR[t] //LO ← GPR[s]/GPR[t])
void DIV(bin_instr_t instruction){
    HI = REGISTERS[instruction.reg.rs] % REGISTERS[instruction.reg.rt];
    LO = REGISTERS[instruction.reg.rs] / REGISTERS[instruction.reg.rt];
}

    //MFHI 0 - - d - 16 Move from HI: GPR[d] ← HI
void MFHI(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd] = HI;
}
    //        MFLO 0 - - d - 18 Move from LO: GPR[d] ← LO
void MFLO(bin_instr_t instruction){
        REGISTERS[instruction.reg.rd] = LO;
    }
    //        AND 0 s t d - 36 Bitwise And: GPR[d] ← GPR[s] ∧ GPR[t]
void AND(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd] = REGISTERS[instruction.reg.rs] & REGISTERS[instruction.reg.rt];
}
    //BOR 0 s t d - 37 Bitwise Or: GPR[d] ← GPR[s] ∨ GPR[t]
void BOR(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd] = REGISTERS[instruction.reg.rs] | REGISTERS[instruction.reg.rt];
}
    //NOR 0 s t d - 39 Bitwise Not-Or: GPR[d] ← ¬(GPR[s] ∨ GPR[t])
void NOR(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd]= ~(REGISTERS[instruction.reg.rs] | REGISTERS[instruction.reg.rt]);
}
    //XOR 0 s t d - 38 Bitwise Exclusive-Or: GPR[d] ← GPR[s] xor GPR[t]
void XOR(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd]= REGISTERS[instruction.reg.rs] ^ REGISTERS[instruction.reg.rt];
}
    //SLL 0 - t d h 0 Shift Left Logical: GPR[d] ← GPR[t] « h
void SLL(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd] = REGISTERS[instruction.reg.rt] << instruction.reg.shift;
}
    //SRL 0 - t d h 3 Shift Right Logical: GPR[d] ← GPR[t] » h
void SRL(bin_instr_t instruction){
    REGISTERS[instruction.reg.rd] = REGISTERS[instruction.reg.rt] >> instruction.reg.shift;
}
    //JR 0 s 0 0 0 8 Jump Register: PC ← GPR[s]
void JR(bin_instr_t instruction){
    PC = REGISTERS[instruction.reg.rs];
}
    //SYSCALL 0 - - - - 12 System Call: (see Table 6)
    /*
void SYSCALL (bin_instr_t instruction) {
    switch((int) instruction.syscall.code){
        case exit_sc:
            exit(0);
            break;
        case print_str_sc:
            printf("%s", &memory.bytes[REGISTERS[instruction.syscall.code]]);
            break;
        case print_char_sc:
            printf("%c", REGISTERS[instruction.syscall.code]);
            break;
        case read_char_sc:
            REGISTERS[instruction.syscall.code] = getchar();
            break;
        case start_tracing_sc:
            isTracing = 1;
            break;
        case stop_tracing_sc:
            isTracing = 0;
            break;
    }
}*/
//endregion
//region IMMEDIATE FORMAT INSTRUCTION:

    //Name op rs rt immed (Explanation)
void ADDI(bin_instr_t instruction) {
    //ADDI 9 s t i Add immediate: GPR[t] ← GPR[s] + sgnExt(i)
     REGISTERS[instruction.immed.rt] = REGISTERS[instruction.immed.rs] + machine_types_sgnExt(instruction.immed.immed);
}

void ANDI(bin_instr_t instruction) {
    //ANDI 12 s t i Bitwise And immediate: GPR[t] ← GPR[s] ∧ zeroExt(i)
	REGISTERS[instruction.immed.rt] = REGISTERS[instruction.immed.rs] && machine_types_zeroExt(instruction.immed.immed);
}

void BORI(bin_instr_t instruction) {
    //BORI 13 s t i Bitwise Or immediate: GPR[t] ← GPR[s] ∨ zeroExt(i)
    REGISTERS[instruction.immed.rt] = REGISTERS[instruction.immed.rs] || machine_types_zeroExt(instruction.immed.immed);
}

void XORI(bin_instr_t instruction) {
	//XORI 14 s t i Bitwise Xor immediate: GPR[t] ← GPR[s] xor zeroExt(i)
    REGISTERS[instruction.immed.rt] = REGISTERS[instruction.immed.rs] ^ machine_types_zeroExt(instruction.immed.immed);
}

void BEQ(bin_instr_t instruction) {
    //BEQ 4 s t o Branch on Equal: if GPR[s] = GPR[t] then PC ← PC + formOffset(o)
    if(REGISTERS[instruction.immed.rs] == REGISTERS[instruction.immed.rt]) {
        PC = PC + machine_types_formOffset(instruction.immed.immed);
    }
}

void BGEZ(bin_instr_t instruction) {
    //BGEZ 1 s 1 o Branch ≥ 0: if GPR[s] ≥ 0 then PC ← PC + formOffset(o)
	if(REGISTERS[instruction.immed.rs] >= REGISTERS[0]) {
		PC = PC + machine_types_formOffset(instruction.immed.immed);
	}
}

void BGTZ(bin_instr_t instruction) {
    //BGTZ 7 s 0 o Branch > 0: if GPR[s] > 0 then PC ← PC + formOffset(o)
	if(REGISTERS[instruction.immed.rs] > REGISTERS[0]) {
		PC = PC + machine_types_formOffset(instruction.immed.immed);
	}
}

void BLEZ(bin_instr_t instruction) {
    //BLEZ 6 s 0 o Branch ≤ 0: if GPR[s] ≤ 0 then PC ← PC + formOffset(o)
	if(REGISTERS[instruction.immed.rs] <= REGISTERS[0]) {
		PC = PC + machine_types_formOffset(instruction.immed.immed);
	}
}

void BLTZ(bin_instr_t instruction) {
    //BLTZ 8 s 0 o Branch < 0: if GPR[s] < 0 then PC ← PC + formOffset(o)
	if(REGISTERS[instruction.immed.rs] < REGISTERS[0]) {
		PC = PC + machine_types_formOffset(instruction.immed.immed);
	}
}

void BNE(bin_instr_t instruction) {
    //BNE 5 s t o Branch Not Equal: if GPR[s] ̸ = GPR[t] then PC ← PC + formOffset(o)
	if(REGISTERS[instruction.immed.rs] != REGISTERS[instruction.immed.rt]) {
		PC = PC + machine_types_formOffset(instruction.immed.immed);
	}
}



//endregion
//region JUMP TYPE INSTRUCTIONS

    //Name op addr (Explanation)
void JMP(address_type a) {
    //JMP 2 a Jump: PC ← formAddress(PC, a)
	PC = machine_types_formAddress(PC, a);
}

void JAL(address_type a) {
    //JAL 3 a Jump and Link: GPR[$ra] ← PC; PC ← formAddress(PC, a)
    setRegister("$ra", PC);
	PC = machine_types_formAddress(PC, a);
}

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