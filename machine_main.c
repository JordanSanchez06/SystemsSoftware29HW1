#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_types.h"
#include "instruction.h"
#include "bof.h"
#include "machine.h"
#include "machine_main.h"

int REGISTERS[NUM_REGISTERS];
address_type PC;

int main(int argc , char **argv) {
    if(strcmp(argv[1],"-p") == 0) {//for -p option

        BOFFILE bf = bof_read_open(argv[2]);
        BOFHeader bh = bof_read_header(bf);

        //print instructions
        printf("Addr Instruction\n");
        PC = bh.text_start_address;
        for(int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++ ) {
            printf("\t%d %s\n", PC, instruction_assembly_form(instruction_read(bf)));
            PC = PC + 4;
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

    PC = bh.text_start_address;

    //initialize important registers
    setRegister("$gp", bh.data_start_address);
    setRegister("$fp", bh.stack_bottom_addr);
    setRegister("$sp", bh.stack_bottom_addr); //i believe frame and stack pointer share this value but i could be wrong.

    //collect instructions
    for(int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++ ) {
        memory.instrs[i] = instruction_read(bf);
    }

    //collect data
    for(int i = 0; i < (bh.data_length/BYTES_PER_WORD); i++ ) {
        memory.words[i] = bof_read_word(bf); //spacing to match test case
    }

    //fetch execute cycle loop
    for(int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++) {
        if(isTracing) {
            printTrace(bh, memory.instrs[i], memory.words);
			PC = PC + 4;
		}

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
				doImmediateInstruction(memory.instrs[i], PC);
                break;
            case jump_instr_type:
                printf("jump");
				doJumpInstruction(memory.instrs[i], i, PC);
                break;
            case error_instr_type:
                printf("error");
                break;
        }
    }
	
    bof_close(bf);
}

 void printTrace(BOFHeader bh,  bin_instr_t instruction, int words[]){
     printf("      PC: %d\n", PC);
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
     printf("==> addr:    %d %s\n", PC, instruction_assembly_form(instruction));
 }

//figures out what instruction to do
void doRegisterInstruction(bin_instr_t instruction) {
    switch((int) instruction.reg.func) {
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
            //SYSCALL(instruction);
            break;
	    default:
	    	bail_with_error("Unkown register instruction", instruction);
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
	        BEQ(instruction, PC);
	        break;
        case BGEZ_O:
	        BGEZ(instruction, PC);
	        break;
        case BGTZ_O:
	        BGTZ(instruction, PC);
	        break;
        case BLEZ_O:
	        BLEZ(instruction, PC);
	        break;
        case BLTZ_O:
	        BLTZ(instruction, PC);
	        break;
        case BNE_O:
        	BNE(instruction, PC);
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

void doJumpInstruction(bin_instr_t instruction, address_type i, address_type PC) {
	switch((int) instruction.jump.op) {
        case JMP_O:
			JMP(i, PC);
            break;
        case JAL_O:
			JAL(i, PC);
            break;
        default:
            bail_with_error("Unkown jump instruction", instruction);
            break;
    }
}
