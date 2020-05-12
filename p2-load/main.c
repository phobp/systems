/*
 *  CS 261 PA2: Mini-ELF loader Main driver
 *
 * Name: Brendan Pho
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "p1-check.h"
#include "p2-load.h"

void dump_memory_helper(memory_t, bool, elf_phdr_t *, int);

int main (int argc, char **argv)
{
    bool header, segments, membrief, memfull = false;
    elf_hdr_t hdr;
    memory_t memory;
    char *filename = NULL;

    // Parse command line and check for failure
    if (!parse_command_line_p2(argc, argv, &header, &segments, &membrief, &memfull, &filename)) {
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

    free(memory);
}
