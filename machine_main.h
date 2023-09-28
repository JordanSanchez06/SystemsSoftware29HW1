#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "instruction.h"
#include "bof.h"
#include "machine.h"

// a size for the memory (2^16 bytes = 64K)
#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)
#define MEMORY_SIZE_IN_WORDS (MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD)

static union mem_u {
    byte_type bytes[MEMORY_SIZE_IN_BYTES];
    word_type words[MEMORY_SIZE_IN_WORDS];
    bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
} memory; //https://webcourses.ucf.edu/courses/1443631/discussion_topics/7460061

//data for each register //extern so we can use it in machine.c where our ADD, SUB, etc. functions will be.
extern int REGISTERS[NUM_REGISTERS];
//use regname_get(index) for name of register

//program counter is needed for some immediate instructions
extern int pc;

extern void printTrace(int pc, BOFHeader bh,  bin_instr_t instruction, int words[]);
extern void doRegisterInstruction(bin_instr_t instruction);
extern void doImmediateInstruction(bin_instr_t instruction, address_type pc);
extern void doJumpInstruction(bin_instr_t instruction, address_type i, address_type pc);