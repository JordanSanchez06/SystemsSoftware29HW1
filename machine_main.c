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

void printTrace();

int main(int argc , char **argv){
    if(strcmp(argv[1],"-p") == 0) {

        BOFFILE bf = bof_read_open(argv[2]);
        BOFHeader bh = bof_read_header(bf);

        //print instructions
        printf("Addr Instruction\n");
        int pc = 0;
        for(int i = 0; i < (bh.text_length / BYTES_PER_WORD); i++ ) {
            printf("\t%d %s\n", pc, instruction_assembly_form(instruction_read(bf)));
            pc += 4;
        }

        //print initial data values.
        int address = bh.data_start_address;
        int wordCount = bh.data_length / BYTES_PER_WORD;
        if(wordCount == 0)
            printf("\t %d: %d ...\n", address,  (bh.data_length / BYTES_PER_WORD));
        for(int i = 0; i < (wordCount); i++ ) {
            printf("\t    %d: %d", address , bof_read_word(bf)); //spacing to match test case
        }
        printf("\n");

        bof_close(bf);

        return 0;
    }


}

 /*void printTrace(int pc){
     printf("\tPC: %d", );
     GPR[$0 ]: 0   	GPR[$at]: 0   	GPR[$v0]: 0   	GPR[$v1]: 0   	GPR[$a0]: 0   	GPR[$a1]: 0
     GPR[$a2]: 0   	GPR[$a3]: 0   	GPR[$t0]: 0   	GPR[$t1]: 0   	GPR[$t2]: 0   	GPR[$t3]: 0
     GPR[$t4]: 0   	GPR[$t5]: 0   	GPR[$t6]: 0   	GPR[$t7]: 0   	GPR[$s0]: 0   	GPR[$s1]: 0
     GPR[$s2]: 0   	GPR[$s3]: 0   	GPR[$s4]: 0   	GPR[$s5]: 0   	GPR[$s6]: 0   	GPR[$s7]: 0
     GPR[$t8]: 0   	GPR[$t9]: 0   	GPR[$k0]: 0   	GPR[$k1]: 0   	GPR[$gp]: 1024	GPR[$sp]: 4096
     GPR[$fp]: 4096	GPR[$ra]: 0
     1024: 0	...
     4096: 0	...
     ==> addr:    0 STRA
 }*/