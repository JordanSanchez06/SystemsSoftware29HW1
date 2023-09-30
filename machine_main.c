#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "instruction.h"
#include "bof.h"
#include "machine.h"
#include "machine_main.h"

// a size for the memory (2^16 bytes = 64K)
#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)
#define MEMORY_SIZE_IN_WORDS (MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD)

//data for each register //extern so we can use it in machine.c where our ADD, SUB, etc. functions will be.
int REGISTERS[NUM_REGISTERS];
//use regname_get(index) for name of register

//isTracing as a global variable
int isTracing;

int HALT;
address_type PC;

void printTrace(BOFHeader bh,  bin_instr_t instruction);
void doRegisterInstruction(bin_instr_t instruction);

int main(int argc , char **argv) {
    if (strcmp(argv[1], "-p") == 0) {//for -p option

        BOFFILE bf = bof_read_open(argv[2]);
        BOFHeader bh = bof_read_header(bf);

        //print instructions
        printf("Addr Instruction\n");
        PC = bh.text_start_address;
        for (int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++) {
            printf("\t%d %s\n", PC, instruction_assembly_form(instruction_read(bf)));
            PC = PC + 4;
        }

        //print initial data values.
        int address = bh.data_start_address;
        int wordCount = bh.data_length / BYTES_PER_WORD;
        for (int i = 0; i < (wordCount); i++) {
            word_type curWord = bof_read_word(bf);
            if(curWord != 0) {
                printf("\t    %d: %d", address, curWord); //spacing to match test case
                address += 4;
            }
            if(i % 4 == 0 && i > 0)
                printf("\n");
        }
        printf("\t %d: %d ...\n", address, 0);
        printf("\n");

        bof_close(bf);

        return 0;
    }

    isTracing = 1;

    BOFFILE bf = bof_read_open(argv[1]);
    BOFHeader bh = bof_read_header(bf);

    PC = bh.text_start_address;

    //initialize important registers
    setRegister("$gp", bh.data_start_address);
    setRegister("$fp", bh.stack_bottom_addr);
    setRegister("$sp", bh.stack_bottom_addr); //i believe frame and stack pointer share this value but i could be wrong.

    //collect instructions
    for (int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++) {
        memory.instrs[i] = instruction_read(bf);
    }

    //collect data
    for(int i = bh.data_start_address; i < (bh.data_length + bh.data_start_address); i+=4) {
        memory.words[i] = bof_read_word(bf); //spacing to match test case
    }

    //fetch execute cycle loop
    while (!HALT) {
        bin_instr_t curInstr = memory.instrs[PC / 4];
        int curInstrType = instruction_type(curInstr);

        if (isTracing)
            printTrace(bh, curInstr);

        PC += 4;

        if (doEnforceInvariants()) {
            fprintf(stderr, "Invariant Violated");
            return 1;
        }
        switch (curInstrType) {
            case reg_instr_type:
                //printf("register instruction");
                doRegisterInstruction(curInstr);
                break;
            case syscall_instr_type:
                //printf("syscall instruction");
                doSyscallInstruction(curInstr);
                break;
            case immed_instr_type:
                //printf("immediate instructions");
                doImmediateInstruction(curInstr, PC);
                break;
            case jump_instr_type:
                //printf("jump");
                doJumpInstruction(curInstr);
                break;
            case error_instr_type:
                //printf("error");
                break;
        }
    }
    bof_close(bf);
}
 void printTrace(BOFHeader bh,  bin_instr_t instruction){
     printf("      PC: %d", PC);
     if(HI || LO)
        printf("        HI: %d           LO: %d",HI, LO);
     printf("\n");
     printf("GPR[$0 ]: %d   	GPR[$at]: %d   	GPR[$v0]: %d   	GPR[$v1]: %d   	GPR[$a0]: %d   	GPR[$a1]: %d\n", REGISTERS[0], REGISTERS[1], REGISTERS[2], REGISTERS[3], REGISTERS[4], REGISTERS[5]);
     printf("GPR[$a2]: %d   	GPR[$a3]: %d   	GPR[$t0]: %d   	GPR[$t1]: %d   	GPR[$t2]: %d   	GPR[$t3]: %d\n", REGISTERS[6], REGISTERS[7], REGISTERS[8], REGISTERS[9], REGISTERS[10], REGISTERS[11]);
     printf("GPR[$t4]: %d   	GPR[$t5]: %d   	GPR[$t6]: %d   	GPR[$t7]: %d   	GPR[$s0]: %d   	GPR[$s1]: %d\n", REGISTERS[12], REGISTERS[13], REGISTERS[14], REGISTERS[15], REGISTERS[16], REGISTERS[17]);
     printf("GPR[$s2]: %d   	GPR[$s3]: %d   	GPR[$s4]: %d   	GPR[$s5]: %d   	GPR[$s6]: %d   	GPR[$s7]: %d\n", REGISTERS[18], REGISTERS[19], REGISTERS[20], REGISTERS[21], REGISTERS[22], REGISTERS[23]);
     printf("GPR[$t8]: %d   	GPR[$t9]: %d   	GPR[$k0]: %d   	GPR[$k1]: %d   	GPR[$gp]: %d	GPR[$sp]: %d\n", REGISTERS[24], REGISTERS[25], REGISTERS[26], REGISTERS[27], REGISTERS[28], REGISTERS[29]);
     printf("GPR[$fp]: %d	GPR[$ra]: %d\n", REGISTERS[30], REGISTERS[31]);
     int i;
     int isZero = 0;
     int printCount = 0;
     for(i = bh.data_start_address; i < (getRegister("$sp")); i+=4 ) {
         if(isZero && memory.words[i] == 0)
             break;
         if(memory.words[i] != 0) {
             isZero = 0;
             printf("    %d: %d", i, memory.words[i]); //spacing to match test case
             printCount++;
         }
         else if(!isZero){
             isZero = 1;
             printf("    %d: %d ...", i, memory.words[i]);
             printCount++;
         }

         if(printCount % 5 == 0 && printCount > 0)
             printf("\n");

     }
     printf("\n");

     isZero = 0;
     printCount = 0;
     for(int i = getRegister("$sp"); i <= getRegister("$fp"); i+=4 ) {
         if(memory.words[i] != 0) {
             isZero = 0;
             printf("    %d: %d", i, memory.words[i]); //spacing to match test case
             printCount++;
         }
         else if(!isZero){
             isZero = 1;
             printf("    %d: %d   ...", i, memory.words[i]);
             printCount++;
         }
         if(printCount % 5 == 0 && printCount > 0)
             printf("\n");
     }
     printf("\n");

     printf("==> addr:    %d %s\n", PC, instruction_assembly_form(instruction));
 }

//figures out what instruction to do
void doRegisterInstruction(bin_instr_t instruction){
    switch((int) instruction.reg.func){
        case ADD_F:
            ADD(instruction);
            break;
        case SUB_F:
            SUB(instruction);
            break;
        case MUL_F:
            MUL(instruction);
            break;
        case DIV_F:
            DIV(instruction);
            break;
        case MFHI_F:
            MFHI(instruction);
            break;
        case MFLO_F:
            MFLO(instruction);
            break;
        case AND_F:
            AND(instruction);
            break;
        case BOR_F:
            BOR(instruction);
            break;
        case NOR_F:
            NOR(instruction);
            break;
        case XOR_F:
            XOR(instruction);
            break;
        case SLL_F:
            SLL(instruction);
            break;
        case SRL_F:
            SRL(instruction);
            break;
        case JR_F:
            JR(instruction);
            break;
        case SYSCALL_F:
            SYSCALL(instruction);
            break;
	    default:
	    	bail_with_error("Unkown register instruction", instruction);
			break;
    }
}

//figures out what syscall instruction to do
void doSyscallInstruction(bin_instr_t instruction){
    switch((int) instruction.syscall.code){
        case exit_sc:
            exit(0);
            break;
        case print_str_sc:
            printf("%s", &memory.bytes[REGISTERS[instruction.syscall.code]]);
            break;
        case print_char_sc:
            setRegister("$v0",fputc(getRegister("$a0"), stdout));
            break;
        case read_char_sc:
            setRegister("$v0", getc(stdin));
            break;
        case start_tracing_sc:
            isTracing = 1;
            break;
        case stop_tracing_sc:
            isTracing = 0;
            break;
    }
}

void doImmediateInstruction(bin_instr_t instruction, address_type PC) {
    switch((int) instruction.immed.op) {
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
	        BEQ(instruction);
	        break;
        case BGEZ_O:
	        BGEZ(instruction);
	        break;
        case BGTZ_O:
	        BGTZ(instruction);
	        break;
        case BLEZ_O:
	        BLEZ(instruction);
	        break;
        case BLTZ_O:
	        BLTZ(instruction);
	        break;
        case BNE_O:
        	BNE(instruction);
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

void doJumpInstruction(bin_instr_t instruction) {
	switch((int) instruction.jump.op) {
        case JMP_O:
			JMP(instruction.jump.addr);
            break;
        case JAL_O:
			JAL(instruction.jump.addr);
            break;
        default:
            bail_with_error("Unkown jump instruction", instruction);
            break;
    }
}

int doEnforceInvariants(){
    if(getRegister("$gp") < 0 ||//0 ≤ GPR[$gp],
            getRegister("$gp") >= getRegister("$sp") || //GPR[$gp] < GPR[$sp]
            getRegister("$sp") > getRegister("$fp") || //GPR[$sp] ≤ GPR[$fp]
            getRegister("$fp") >= MEMORY_SIZE_IN_BYTES ||//GPR[$fp] < MAX_STACK_HEIGHT,
            PC < 0 ||//0 ≤ PC,
            PC > MEMORY_SIZE_IN_BYTES || //PC < MEMORY_SIZE_IN_BYTES
            getRegister("$0") != 0// GPR[0] = 0
    )return 1;
    else return 0;
}

