/*
 * CS 261: Main driver
 *
 * Name: Brendan Pho
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "p1-check.h"
#include "p2-load.h"
#include "p3-disas.h"
#include "p4-interp.h"

void dump_memory_helper(memory_t, bool, elf_phdr_t *, int);
void register_byte(uint8_t, y86_rnum_t *, y86_rnum_t *);
bool memory_bounds(uint8_t, y86_register_t);
void register_print(y86_regname_t);

int main (int argc, char **argv)
{
    bool header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug = false;
    elf_hdr_t hdr;
    memory_t memory;
    char *filename = NULL;

    // Parse command line and check for failure
    if (!parse_command_line_p4(argc, argv, &header, &segments, &membrief, &memfull, &disas_code, &disas_data, &exec_normal, &exec_debug, &filename)) {
        exit(EXIT_FAILURE);
    }

    // Parsing succeeds, but no file name
    if (!filename) {
        exit(EXIT_FAILURE);
    }

    // Open the input file
    FILE *input = fopen(filename, "r");

    // Can not open file
    if (!input) {
        printf("Failed to read file\n");
        exit(EXIT_FAILURE);
    }

    memory = calloc(MEMSIZE, sizeof(uint8_t));
    if (!memory) {
        exit(EXIT_FAILURE);
    }

    // Read the bytes of the header
    if (!read_header(input, &hdr)) {
        printf("Failed to read file\n");
        free(memory);
        exit(EXIT_FAILURE);
    }

    elf_phdr_t phdrArray[hdr.e_num_phdr];
    int i;

    // Read program headers from the program header array
    for (i = 0; i < hdr.e_num_phdr; i++) {
        if (!read_phdr(input, hdr.e_phdr_start + (i * sizeof(elf_phdr_t)), &phdrArray[i])) {
            printf("Failed to read file\n");
            free(memory);
            exit(EXIT_FAILURE);
        }
    }

    // Load segments
    for (i = 0; i < hdr.e_num_phdr; i++) {
        load_segment(input, memory, phdrArray[i]);
    }

    // Call dump_header to print information if all conditions are met
    if (header) {
        dump_header(hdr);
    }

    // Dump program header information
    if (segments) {
        dump_phdrs(hdr.e_num_phdr, phdrArray);
    }

    if (membrief || memfull) {
        dump_memory_helper(memory, membrief, phdrArray, hdr.e_num_phdr);
    }
    
    // If -d call disassemble_code()
    if (disas_code) {
        printf("Disassembly of executable contents:\n");
        for (int i = 0; i < hdr.e_num_phdr; i++) {
            if (phdrArray[i].p_type == 1) {
                disassemble_code(memory, &phdrArray[i], &hdr);
                printf("\n");
            }
        }
    }

    // If -D call disassemble_data() or disassemble_rodata() 
    if (disas_data) {
        printf("Disassembly of data contents:\n");
        for (int i = 0; i < hdr.e_num_phdr; i++) {
            if (phdrArray[i].p_type == 0 && phdrArray[i].p_flag == RW) {
                disassemble_data(memory, &phdrArray[i]);
                printf("\n");
            }

            if (phdrArray[i].p_type == 0 && phdrArray[i].p_flag == RO) {
                disassemble_rodata(memory, &phdrArray[i]);
                printf("\n");
            }
        }
    }
    
    // -e funtionality
    if (exec_normal || exec_debug) {
        y86_t *cpu = calloc(1, sizeof(y86_t));
        printf("Beginning execution at 0x%04x\n", hdr.e_entry);
        cpu->pc = hdr.e_entry;
        cpu->stat = AOK;
        
        // fetch, decode, execute, mem, wb, update pc loop
        bool cond = false;
        int count = 0;
        y86_inst_t inst;
        y86_register_t valA;
        y86_register_t valE;

        // -E functionality
        if (exec_debug) {
            cpu_state(cpu);
        }
        while (cpu->stat == AOK) {
            inst = fetch(cpu, memory);
            if (exec_debug) {
                printf("\nExecuting: ");
                disassemble(inst);
            }
            valE = decode_execute(cpu, &cond, &inst, &valA);
            memory_wb_pc(cpu, memory, cond, &inst, valE, valA);
            count++;
            if (exec_debug) {
                cpu_state(cpu);
            }
            if (count > 1000) cpu->stat = ADR;
        }
        if (!exec_debug) cpu_state(cpu);
        printf("Total execution count: %d\n\n", count);
        free(cpu);
    }

    if (exec_debug) {
       dump_memory(memory, 0, MEMSIZE); 
       printf("\n");
    }

    free(memory);
}

void dump_memory_ (memory_t memory, uint16_t start, uint16_t end)
{
    printf("Contents of memory from %04x to %04x:", start, end);
    int i = start - (start % 16);
    int j = 0;
    while (i < end) {
        if (j == 0) {
            printf("\n  %04x ", i);
        }
        printf("%s%02x", j == 8 ? "  " : " ", memory[i]);

        j = ++i % 16;
    }
    printf("\n");
}

void dump_memory_helper(memory_t memory, bool isBrief, elf_phdr_t *phdr, int length) {
   if (!isBrief) {
        dump_memory(memory, 0, MEMSIZE);
   } else {
        int i;
        for (i = 0; i < length; i++) {
            dump_memory(memory, phdr[i].p_vaddr, phdr[i].p_vaddr + phdr[i].p_filesz);
        }
   }
}




