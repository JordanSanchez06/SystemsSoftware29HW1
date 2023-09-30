#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "instruction.h"
#include "bof.h"



//data for each register //extern so we can use it in machine.c where our ADD, SUB, etc. functions will be.
extern int REGISTERS[NUM_REGISTERS];
//use regname_get(index) for name of register

//program counter is needed for some immediate instructions
extern address_type PC;

//special registers for mul and div
extern address_type HI;
extern address_type LO;

extern void printTrace(BOFHeader bh,  bin_instr_t instruction);
extern void doRegisterInstruction(bin_instr_t instruction);
extern void doImmediateInstruction(bin_instr_t instruction, address_type PC);
extern void doJumpInstruction(bin_instr_t instruction);
extern void doSyscallInstruction(bin_instr_t instruction);
extern int doEnforceInvariants();
extern int getRegister(char * name);
extern void LBU(bin_instr_t intrsuction);
extern void LW(bin_instr_t intrsuction);
extern void SB(bin_instr_t intrsuction);
extern void SW(bin_instr_t intrsuction);