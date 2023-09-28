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
int REGISTERS[NUM_REGISTERS];
//use regname_get(index) for name of register

//program counter is needed for some immediate instructions
int pc;

void printTrace(int pc, BOFHeader bh,  bin_instr_t instruction, int words[]);
void doRegisterInstruction(bin_instr_t instruction);
void doImmediateInstruction(bin_instr_t instruction, int pc);
void doJumpInstruction(bin_instr_t instruction, int pc);

int main(int argc , char **argv){
    if(strcmp(argv[1],"-p") == 0) {//for -p option

        BOFFILE bf = bof_read_open(argv[2]);
        BOFHeader bh = bof_read_header(bf);

        //print instructions
        printf("Addr Instruction\n");
        pc = bh.text_start_address;
        for(int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++ ) {
            printf("\t%d %s\n", pc, instruction_assembly_form(instruction_read(bf)));
            pc += 4;
        }

        //print initial data values.
        int address = bh.data_start_address;
        int wordCount = bh.data_length / BYTES_PER_WORD;
        for(int i = 0; i < (wordCount); i++ ) {
            printf("\t    %d: %d", address , bof_read_word(bf)); //spacing to match test case
            address += 4;
        }
        printf("\t %d: %d ...\n", address, 0);
        printf("\n");

        bof_close(bf);

        return 0;
    }

    int isTracing = 1;

    BOFFILE bf = bof_read_open(argv[1]);
    BOFHeader bh = bof_read_header(bf);

    pc = bh.text_start_address;

    //initialize important registers
    setRegister("$gp", bh.data_start_address);
    setRegister("$fp", bh.stack_bottom_addr);
    setRegister("$sp", bh.stack_bottom_addr); //i believe frame and stack pointer share this value but i could be wrong.

    //collect instructions
    for(int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++ ) {
        memory.instrs[i] = instruction_read(bf);
    }

    //collect words
    int wordCount = bh.data_length / BYTES_PER_WORD;
    int words[wordCount];
    for(int i = 0; i < (wordCount); i++ ) {
        words[i] = bof_read_word(bf); //spacing to match test case
    }

    //fetch execute cycle loop
    for(int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++) {
        if(isTracing)
            printTrace(pc, bh, memory.instrs[i], words);

        //fetch and execute
        int curInstrType = instruction_type(memory.instrs[i]);
        printf("%d", curInstrType);
        switch(curInstrType){
            case reg_instr_type:
                printf("register instruction");
                doRegisterInstruction(memory.instrs[i]);
                break;
            case syscall_instr_type:
                printf("syscall instruction");
                break;
            case immed_instr_type:
                printf("immediate instructions");
				doImmediateInstruction(memory.instrs[i], pc);
                break;
            case jump_instr_type:
                printf("jump");
				doJumpInstruction(i, pc)
                break;
            case error_instr_type:
                printf("error");
                break;
        }
    }





    bof_close(bf);
}

 void printTrace(int pc, BOFHeader bh,  bin_instr_t instruction, int words[]){
     printf("      PC: %d\n", pc);
     printf("GPR[$0 ]: %d   	GPR[$at]: %d   	GPR[$v0]: %d   	GPR[$v1]: %d   	GPR[$a0]: %d   	GPR[$a1]: %d\n", REGISTERS[0], REGISTERS[1], REGISTERS[2], REGISTERS[3], REGISTERS[4], REGISTERS[5]);
     printf("GPR[$a2]: %d   	GPR[$a3]: %d   	GPR[$t0]: %d   	GPR[$t1]: %d   	GPR[$t2]: %d   	GPR[$t3]: %d\n", REGISTERS[6], REGISTERS[7], REGISTERS[8], REGISTERS[9], REGISTERS[10], REGISTERS[11]);
     printf("GPR[$t4]: %d   	GPR[$t5]: %d   	GPR[$t6]: %d   	GPR[$t7]: %d   	GPR[$s0]: %d   	GPR[$s1]: %d\n", REGISTERS[12], REGISTERS[13], REGISTERS[14], REGISTERS[15], REGISTERS[16], REGISTERS[17]);
     printf("GPR[$s2]: %d   	GPR[$s3]: %d   	GPR[$s4]: %d   	GPR[$s5]: %d   	GPR[$s6]: %d   	GPR[$s7]: %d\n", REGISTERS[18], REGISTERS[19], REGISTERS[20], REGISTERS[21], REGISTERS[22], REGISTERS[23]);
     printf("GPR[$t8]: %d   	GPR[$t9]: %d   	GPR[$k0]: %d   	GPR[$k1]: %d   	GPR[$gp]: %d	GPR[$sp]: %d\n", REGISTERS[25], REGISTERS[25], REGISTERS[26], REGISTERS[27], REGISTERS[28], REGISTERS[29]);
     printf("GPR[$fp]: %d	GPR[$ra]: %d\n", REGISTERS[30], REGISTERS[31]);
     int address = bh.data_start_address;
     int wordCount = bh.data_length / BYTES_PER_WORD;
     for(int i = 0; i < (wordCount); i++ ) {
         printf("     %d: %d", address , words[i]); //spacing to match test case
         address += 4;
     }
     printf("     %d: %d ...", address,  0); //TODO is this zero everytime
     printf("\n");
     printf("     %d: 0	...\n", bh.stack_bottom_addr); //TODO is that zero everytime? What do ... signify?
     printf("==> addr:    %d %s\n", pc, instruction_assembly_form(instruction));
 }

//figures out what instruction to do
void doRegisterInstruction(bin_instr_t instruction) {
    switch((int) instruction.reg.func) {
        case ADD_F:
            ADD(instruction);
            break;
    }
}

void doImmediateInstruction(bin_instr_t instruction, int pc) {
    switch((int) instruction.immed.func) {
        case ADDI_O:
	        ADDI(instruction);
	        break;
        case ANDI_O:
	        ANDI(instruction);
	        break;
        case BORI_O:
            BORI(instruction);
	        break;
        case XORI_O:
	        XORI(instruction);
	        break;
        case BEQ_O:
	        BEQ(instruction, pc);
	        break;
        case BGEZ_O:
	        BGEZ(instruction, pc);
	        break;
        case BGTZ_O:
	        BGTZ(instruction, pc);
	        break;
        case BLEZ_O:
	        BLEZ(instruction, pc);
	        break;
        case BLTZ_O:
	        BLTZ(instruction, pc);
	        break;
        case BNE_O:
        	BNE(instruction, pc);
	        break;
        case LBU_O:
        	LBU(instruction);
	        break;
        case LW_O:
        	LW(instruction);
	        break;
        case SB_O:
        	SB(instruction);
	        break;
        case SW_O:
        	SW(instruction);
		    break;
        default:
			bail_with_error("Unknown immediate instruction", instruction);
            break;
    }
}

void doJumpInstruction(address_type i, address_type pc) {
	switch((int)) instruction.jump.func) {
        case JMP_O:
			JMP(i, pc);
            break;
        case JAL_O:
			JAL(i, pc);
            break;
        default:
            bail_with_error("Unkown jump instruction", instruction);
            break;
    }
}
