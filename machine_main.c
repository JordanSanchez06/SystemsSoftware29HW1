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

int main(int argc , char **argv){
    if(strcmp(argv[1],"-p") == 0) {

        BOFFILE bf = bof_read_open(argv[2]);

        BOFHeader bh = bof_read_header(bf);

        printf("%s\n", instruction_assembly_form(instruction_read(bf)));
        printf("%s\n", instruction_assembly_form(instruction_read(bf)));
        printf("%s\n", instruction_assembly_form(instruction_read(bf)));

        bof_close(bf);
    }
}