#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "instruction.h"
#include "bof.h"

// a size for the memory (2^16 bytes = 64K)
#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)
#define MEMORY_SIZE_IN_WORDS (MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD)

//isTracing as a global variable
int isTracing;

static union mem_u {
    byte_type bytes[MEMORY_SIZE_IN_BYTES];
    word_type words[MEMORY_SIZE_IN_WORDS];
    bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
} memory; //https://webcourses.ucf.edu/courses/1443631/discussion_topics/7460061

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