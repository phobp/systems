/*
 * CS 261 PA2: Mini-ELF loader
 *
 * Name: Brendan Pho
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p2-load.h"

void dump_memory_helper(memory_t, bool, elf_phdr_t *, int);

void usage_p2 ()
{
    printf("Usage: y86 <option(s)> mini-elf-file\n");
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
    printf("  -a      Show all with brief memory\n");
    printf("  -f      Show all with full memory\n");
    printf("  -s      Show the program headers\n");
    printf("  -m      Show the memory contents (brief)\n");
    printf("  -M      Show the memory contents (full)\n");
}

bool parse_command_line_p2 (int argc, char **argv,
        bool *header, bool *segments, bool *membrief, bool *memfull,
        char **file)
{
    if (!argv || !header || !segments || !membrief || !memfull || !file) {
        return false;
    }
    //checks for NULL pointers

    opterr = 0; /* Prevents getopt() from printing error messages */
    char *optionStr = "+hHafsmM";
    char opt;

    while ( (opt = getopt(argc, argv, optionStr) ) != - 1) {
        switch (opt) {
            case 'h':
                usage_p2();
                *header = false;
                *file = NULL;
                return true;
                break;
            case 'H':
                *header = true;
                break;
            case 'a':
                *header = true;
                *segments = true;
                *membrief = true;
                break;
            case 'f':
                *header = true;
                *segments =true;
                *memfull = true;
                break;
            case 's':
                *segments = true;
                break;
            case 'm':
                *membrief = true;
                break;
            case 'M':
                *memfull = true;
                break;
            case '?':
                printf("Failed to read file\n");
                return false;
            default: // invalid options found
                usage_p2();
                *header = false;
                *segments = false;
                *membrief = false;
                *memfull = false;
                *file = NULL;
                printf("Failed to read file\n");
                return false;
        }
    }

    if (*membrief && *memfull)  {
        usage_p2();
        return false;
    }
    if (optind != argc - 1) { // Exactly ONE Filename must be passed at the end
        usage_p2(); // either no Filename, or more than one file name present
        return false;
    }
    *file = argv[optind];
    
    return true;
}

bool read_phdr (FILE *file, uint16_t offset, elf_phdr_t *phdr)
{
    if (!file || !phdr) {
        return false;
    }

    fseek(file, 0, SEEK_END);
    int end = ftell(file);

    if (offset + sizeof(elf_phdr_t) > end) {
        return false;
    }

    if (fseek(file, offset, SEEK_SET) == -1) {
        return false;    
    }

    if ( !fread(phdr, sizeof(elf_phdr_t), 1, file ) ) {
        return false;
    } // The desired number of bytes could not be read from the file

    if ( phdr -> magic != 0xdeadbeef) {
        return false;
    }

    if ( phdr -> p_vaddr >= MEMSIZE ) {
        return false;
    }

    return true;
}

void dump_phdrs (uint16_t numphdrs, elf_phdr_t phdr[])
{
    int i;
    elf_phdr_t x;
    char *type;
    char *flag;
    printf(" Segment   Offset    VirtAddr  FileSize  Type      Flag\n");
    for (i = 0; i < numphdrs; i++) {
        x = phdr[i];
        if (x.p_type == 0) {
            type = "DATA ";
        } else if (x.p_type == 1) {
            type = "CODE ";
        } else if (x.p_type == 2) {
            type = "STACK";
        } else if (x.p_type == 3) {
            type = "HEAP ";
        } else {
            type = "UNKNOWN";
        } // determine the type based on p_type's number
        
        if (x.p_flag == RO) {
            flag = "R   ";
        }
        else if (x.p_flag == RX) {
            flag = "R X ";
        }
        else if (x.p_flag == RW) {
            flag = "RW  ";
        } else {
            flag = "";
        } // determine the flag

        printf("  %02d       0x%04x    0x%04x    0x%04x    %s     %s\n",
            i, x.p_offset, x.p_vaddr, x.p_filesz, type, flag); // print elf_phdr_t data
    }
}

bool load_segment (FILE *file, memory_t memory, elf_phdr_t phdr)
{
    if (!file || !memory) {
        return false;
    }

    if ( phdr.magic != 0xdeadbeef ) {
        return false;
    }

    if ( phdr.p_vaddr + phdr.p_filesz >= MEMSIZE) {
        return false;
    } // check if the file size surpasses the memory size starting at the virtual address

    char buffer[phdr.p_filesz];

    if (fseek(file, phdr.p_offset, SEEK_SET) == -1) {
        return false;    
    }

    if (!fread(buffer, sizeof(char), phdr.p_filesz, file)) {

        return false;        
    } // The desired number of bytes could not be read from the file

    memcpy(memory + phdr.p_vaddr, buffer, phdr.p_filesz);
    return true;
}

void dump_memory (memory_t memory, uint16_t start, uint16_t end)
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
