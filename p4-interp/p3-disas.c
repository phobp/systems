/*
 * CS 261 PA3: Mini-ELF disassembler
 *
 * Name: Brendan Pho 
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "p3-disas.h"

void register_byte(uint8_t, y86_rnum_t *, y86_rnum_t *);
bool memory_bounds(uint8_t, y86_register_t);
void register_print(y86_regname_t);
void dump_memory_helper(memory_t memory, bool isBrief, elf_phdr_t *phdr, int length);

void usage_p3 ()
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
    printf("  -d      Disassemble code contents\n");
    printf("  -D      Disassemble data contents\n");
}

bool parse_command_line_p3 (int argc, char **argv,
        bool *header, bool *segments, bool *membrief, bool *memfull,
        bool *disas_code, bool *disas_data, char **file)
{
    if (!argv || !header || !segments || !membrief || !memfull
    || !disas_code || !disas_data || !file) {
        return false;
    }

    opterr = 0; /* Prevents getopt() from printing error messages */
    char *optionStr = "+hHafsmMdD";
    char opt;
    *header = false;
    *segments = false;
    *membrief = false;
    *memfull = false;
    *disas_code = false;
    *disas_data = false;
    
    while ( (opt = getopt(argc, argv, optionStr) ) != - 1) {
        switch (opt) {
            case 'h':
                usage_p3();
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
            case 'd':
                *disas_code = true;
                break;
            case 'D':
                *disas_data = true;
                break;
            case '?':
                printf("Failed to read file\n");
                return false;
            default: // invalid options found
                usage_p3();
                *header = false;
                *segments = false;
                *membrief = false;
                *memfull = false;
                *disas_code = false;
                *disas_data = false;
                *file = NULL;
                printf("Failed to read file\n");
                return false;
        }
    }

    if (*membrief && *memfull)  {
        usage_p3();
        return false;
    }
    if (optind != argc - 1) { // Exactly ONE Filename must be passed at the end
        usage_p3(); // either no Filename, or more than one file name present
        return false;
    }

    *file = argv[optind];
    return true;
}

y86_inst_t fetch (y86_t *cpu, memory_t memory)
{
    y86_inst_t ins;
    memset(&ins, 0, sizeof(y86_inst_t));
    uint8_t inst_type;
    uint8_t inst_subtype;

    if (!memory) {
        ins.type = INVALID;
        return ins;
    }

    if (cpu->pc >= MEMSIZE) {
	ins.type = INVALID;
        return ins;
    }
    inst_type = (memory[cpu->pc] & 0xf0) >> 4;
    inst_subtype = memory[cpu->pc] & 0x0f;

    ins.type = inst_type;
    ins.opcode = memory[cpu->pc];

    switch(inst_type) { // Type of inst 
   
    case RET:           // Set instruction values based on type
        ins.size = 1;
        
        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0) {
            ins.type = INVALID;
            return ins;
        }
        break;

    case HALT:     
        ins.size = 1;
        
        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0) {
            ins.type = INVALID;
            return ins;
        }
        break;

    case NOP:
        ins.size = 1;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0) {
            ins.type = INVALID;
            return ins;
        }
        break;

    case CMOV:
        ins.size = 2;
        ins.type = CMOV;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype > 6) {
            ins.type = INVALID;
            return ins;
        }

        ins.cmov = inst_subtype;
        register_byte(memory[cpu->pc + 1], &ins.ra, &ins.rb);

        if (ins.ra >= BADREG || ins.rb >= BADREG) {
            ins.type = INVALID;
            ins.opcode = memory[cpu->pc];
            return ins;
        }
        break;

    case IRMOVQ:
        ins.size = 10;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0 
            || (memory[cpu->pc + 1] & 0xf0) != 0xf0) {
            ins.type = INVALID;
            return ins;
        }

        register_byte(memory[cpu->pc + 1], NULL, &ins.rb);
        ins.value = *((uint64_t *) (memory + cpu->pc + 2));
        if (ins.rb >= BADREG) {
            ins.type = INVALID;
            ins.opcode = memory[cpu->pc];
            return ins;
        }


        break;

    case RMMOVQ:
        ins.size = 10;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0) {
            ins.type = INVALID;
            return ins;
        }

        register_byte(memory[cpu->pc + 1], &ins.ra, &ins.rb);
        ins.d = *((uint64_t *) (memory + cpu->pc + 2));
        if (ins.ra >= BADREG || ins.rb >= BADREG + 1) {
            ins.type = INVALID;
            ins.opcode = memory[cpu->pc];
            return ins;
        }


        break;

    case MRMOVQ:
        ins.size = 10;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0) {
            ins.type = INVALID;
            return ins;
        }

        register_byte(memory[cpu->pc + 1], &ins.ra, &ins.rb);
        ins.d = *((uint64_t *) (memory + cpu->pc + 2));
        if (ins.ra >= BADREG || ins.rb >= BADREG + 1) {
            ins.type = INVALID;
            ins.opcode = memory[cpu->pc];
            return ins;
        }


        break;

    case OPQ:
        ins.size = 2;
        ins.type = OPQ;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype > 3) {
            ins.type = INVALID;
            return ins;
        }

        ins.op = inst_subtype;
        register_byte(memory[cpu->pc + 1], &ins.ra, &ins.rb);
        if (ins.ra >= BADREG || ins.rb >= BADREG) {
            ins.type = INVALID;
            ins.opcode = memory[cpu->pc];
            return ins;
        }


        break;

    case JUMP:
        ins.size = 9;
        ins.type = JUMP;
        
        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype > 6) {
            ins.type = INVALID;
            return ins;
        }

        ins.jump = inst_subtype;
        ins.dest = *((uint64_t *) (memory + cpu->pc + 1));

        break;

    case CALL:
        ins.size = 9;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0) {
            ins.type = INVALID;
            return ins;
        }

        ins.dest = *((uint64_t *) (memory + cpu->pc + 1));
        break;

    case PUSHQ:
        ins.size = 2;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0 
            || (memory[cpu->pc + 1] & 0x0f) != 0x0f) {
            ins.type = INVALID;
            return ins;
        }

        register_byte(memory[cpu->pc + 1], &ins.ra, NULL);
        if (ins.ra >= BADREG) {
            ins.type = INVALID;
            ins.opcode = memory[cpu->pc];
            return ins;
        }

        break;

    case POPQ:
        ins.size = 2;

        if (!memory_bounds(ins.size, cpu->pc) || inst_subtype != 0 
            || (memory[cpu->pc + 1] & 0x0f) != 0x0f) {
            ins.type = INVALID;
            return ins;
        }

        register_byte(memory[cpu->pc + 1], &ins.ra, NULL);
        if (ins.ra >= BADREG) {
            ins.type = INVALID;
            ins.opcode = memory[cpu->pc];
            return ins;
        }

        break;

    default:
        ins.opcode = memory[cpu->pc];
        ins.type = INVALID;
        break;
    }

    return ins;
}

void disassemble (y86_inst_t inst)
{

    switch(inst.type) {     // Type of instruction
        
        case HALT:          // For each type, print instruction 
            printf("halt");
            break;
        case NOP:
            printf("nop");
            break;
        case CMOV:

            switch(inst.cmov) { // For each subtype, print instruction
                
                case RRMOVQ:
                    printf("rrmovq ");
                    break;
                case CMOVLE:
                    printf("cmovle ");
                    break;
                case CMOVL:
                    printf("cmovl ");
                    break;
                case CMOVE:
                    printf("cmove ");
                    break;
                case CMOVNE:
                    printf("cmovne ");
                    break;
                case CMOVGE:
                    printf("cmovge ");
                    break;
                case CMOVG:
                    printf("cmovg ");
                    break;
                default:
                    printf("badcmov");
                    break;
            }

            register_print(inst.ra);
            printf(", ");
            register_print(inst.rb);
            break;
        case IRMOVQ:
            printf("irmovq ");
            printf("0x%lx, ", inst.value);
            register_print(inst.rb);
            break;
        case RMMOVQ:
            printf("rmmovq ");
            register_print(inst.ra);
            printf(", 0x%lx(", inst.d);
            register_print(inst.rb);
            printf(")");
            break;
        case MRMOVQ:
            printf("mrmovq 0x%lx(", inst.d);
            register_print(inst.rb);
            printf("), ");
            register_print(inst.ra);
            break;
        case OPQ:
            
            switch(inst.op) {
                case ADD:
                    printf("addq ");
                    break;
                case SUB:
                    printf("subq ");
                    break;
                case AND:
                    printf("andq ");
                    break;
                case XOR:
                    printf("xorq ");
                    break;
                default:
                    printf("badop");
                    break;
            }
            register_print(inst.ra);
            printf(", ");
            register_print(inst.rb);

            break;
        case JUMP:
            
            switch(inst.jump) {
                case JMP:
                    printf("jmp ");
                    break;
                case JLE:
                    printf("jle ");
                    break;
                case JL:
                    printf("jl ");
                    break;
                case JE:
                    printf("je ");
                    break;
                case JNE:
                    printf("jne ");
                    break;
                case JGE:
                    printf("jge ");
                    break;
                case JG:
                    printf("jg ");
                    break;
                default:
                    printf("badjump");
                    break;
            }

            printf("0x%lx", inst.dest);
            break;
        case CALL:
            printf("call 0x%lx", inst.dest);
            break;
        case RET:
            printf("ret");
            break;
        case PUSHQ:
            printf("pushq ");
            register_print(inst.ra);
            break;
        case POPQ:
            printf("popq ");
            register_print(inst.ra);
            break;
        default:
            printf("invalid");
            break;
    }

    printf("\n");
}

void disassemble_code (memory_t memory, elf_phdr_t *phdr, elf_hdr_t *hdr)
{
    if (!phdr || !hdr) return;

    y86_t cpu;
    y86_inst_t inst;

    
    cpu.pc = phdr->p_vaddr;
    
    printf("  0x%03lx:                      | .pos 0x%03lx code\n", cpu.pc, cpu.pc);
    while (cpu.pc < phdr->p_filesz + phdr->p_vaddr) {

        if (cpu.pc == hdr->e_entry) {
            printf("  0x%03lx:                      | _start:\n", cpu.pc);
        }

        inst = fetch(&cpu, memory);
        if (inst.type == INVALID) {
            printf("Invalid opcode: 0x%02x\n", inst.opcode);
            return;
        }
        printf("  0x%03lx: ", cpu.pc);

        for (uint8_t i = 0; i < inst.size; i++) {
            printf("%02x", memory[cpu.pc + i]);
        }

        for (int i = 0; i < 10 - inst.size; i++) {
            printf("  ");
        }
        
        printf(" |   ");
        
        disassemble(inst);
        cpu.pc += inst.size;
    }
}

void disassemble_data (memory_t memory, elf_phdr_t *phdr)
{
    if (!phdr) {
        return;
    }

    y86_t cpu;
    cpu.pc = phdr->p_vaddr;

    printf("  0x%03lx:                      | .pos 0x%03lx data\n", cpu.pc, cpu.pc);

    while (cpu.pc < phdr->p_filesz + phdr->p_vaddr) {
        
        printf("  0x%03lx: ", cpu.pc);
        for (uint8_t i = 0; i < 8; i++) {
            printf("%02x", memory[cpu.pc + i]);
        }

        printf("     |   .quad 0x%lx\n", *((uint64_t * )(memory + cpu.pc)));
        cpu.pc += 8;
    }

}

void disassemble_rodata (memory_t memory, elf_phdr_t *phdr)
{
    
    if (!phdr) {
        return;
    }

    uint32_t pc = phdr->p_vaddr;

    printf("  0x%03x:                      | .pos 0x%03x rodata\n", pc, pc);
   
    while (pc < phdr->p_filesz + phdr->p_vaddr) {    
        char *str = (char *) memory + pc; // start of string
        int strl = strlen(str);
        char *curr = str; // current character to print        
        char *eos = (char *) str + strl; // the "\0"
        
        printf("  0x%03x: ", pc);
        for (int i = 0; i < 10; i++) {
            if (curr <= eos) {
                printf("%02x", *curr++);
                pc+=1;
            } else {
                printf("  ");
            }
        }
        printf(" |   .string \"%s\"\n", str);
        while (curr <= eos) {
            // if more than 10 chars:
            printf("  0x%03x: ", pc);
            for (int i = 0; i < 10; i++) {
                if (curr <= eos) {
                    printf("%02x", *curr++);
                    pc+=1;
                } else {
                    printf("  ");
                }
            }
            printf(" |\n");
        }
    } 
}

void register_byte(uint8_t byte, y86_rnum_t *ra, y86_rnum_t *rb) {

    if (ra) {
        *ra = (byte & 0xf0) >> 4;
    }

    if (rb) {
        *rb = byte & 0x0f;
    }
} // gets the register byte

bool memory_bounds(uint8_t size, y86_register_t pc) {
    if (pc + size - 1 < MEMSIZE) {
        return true;
    }

    return false;
} // checks if within the bounds of memory

void register_print(y86_regname_t reg) {
    
    printf("%%");

    switch (reg) {
        case RAX:
            printf("rax");
            break;
        case RCX:
            printf("rcx");
            break;
        case RDX:
            printf("rdx");
            break;
        case RBX:
            printf("rbx");
            break;
        case RSP:
            printf("rsp");
            break;
        case RBP:
            printf("rbp");
            break;
        case RSI:
            printf("rsi");
            break;
        case RDI:
            printf("rdi");
            break;
        case R8:
            printf("r8");
            break;
        case R9:
            printf("r9");
            break;
        case R10:
            printf("r10");
            break;
        case R11:
            printf("r11");
            break;
        case R12:
            printf("r12");
            break;
        case R13:
            printf("r13");
            break;
        case R14:
            printf("r14");
            break;
        default:
            printf("badreg");
            break;
    } // prints the type of register
}

