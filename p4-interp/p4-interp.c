/*
 * CS 261 PA4: Mini-ELF interpreter
 *
 * Name: Brendan Pho
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "p4-interp.h"

void print_regname(y86_regname_t);
y86_register_t *get_cpu_reg(y86_t *, y86_rnum_t);
//=======================================================================

void usage_p4 ()
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
    printf("  -e      Execute program\n");
    printf("  -E      Execute program (debug trace mode)\n");
}

//=======================================================================

bool parse_command_line_p4 
    (   int argc         , char **argv      ,
        bool *header     , bool *segments   , bool *membrief , 
        bool *memfull    , bool *disas_code , bool *disas_data ,
        bool *exec_normal, bool *exec_debug , char **file 
    )
{
    if (!argv || !header || !segments || !membrief || !memfull
    || !disas_code || !disas_data || !exec_normal || !exec_debug || !file) {
        return false;
    }

    opterr = 0; /* Prevents getopt() from printing error messages */
    char *optionStr = "+hHafsmMdDeE";
    char opt;
    *header = false;
    *segments = false;
    *membrief = false;
    *memfull = false;
    *disas_code = false;
    *disas_data = false;
    *exec_normal = false;
    *exec_debug = false;
    
    while ( (opt = getopt(argc, argv, optionStr) ) != - 1) {
        switch (opt) {
            case 'h':
                usage_p4();
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
            case 'e':
                *exec_normal = true;
                break;
            case 'E':
                *exec_debug = true;
                break;
            case '?':
                printf("Failed to read file\n");
                return false;
            default: // invalid options found
                usage_p4();
                *header = false;
                *segments = false;
                *membrief = false;
                *memfull = false;
                *disas_code = false;
                *disas_data = false;
                *exec_normal = false;
                *exec_debug = false;
                *file = NULL;
                printf("Failed to read file\n");
                return false;
        }
    }

    if (*membrief && *memfull)  {
        usage_p4();
        return false;
    }
    if (optind != argc - 1) { // Exactly ONE Filename must be passed at the end
        usage_p4(); // either no Filename, or more than one file name present
        return false;
    }

    *file = argv[optind];
    return true;
}

//=======================================================================

void cpu_state( const y86_t *cpu )
{
    if (!cpu) {
        return;
    }
    y86_rnum_t r = 0;
    y86_regname_t rn = RAX;
      
    printf("Y86 CPU state:\n");
    printf("  %%rip: %016lx   flags: Z%x S%x O%x     ", cpu->pc, cpu->zf, cpu->sf, cpu->of);
    
    switch(cpu->stat) {
        case AOK:
            printf("AOK\n");
            break;
        case HLT:
            printf("HLT\n");
            break;
        case ADR:
            printf("ADR\n");
            break;
        case INS:
            printf("INS\n");
            break;
        default:
            break;
    }

    while (rn < R14) {
        printf("  ");
        print_regname(rn++);
        printf(": %016lx    ", *get_cpu_reg((y86_t *)cpu, r++));
        print_regname(rn++);
        printf(": %016lx\n", *get_cpu_reg((y86_t *)cpu, r++));
    }

    printf("  ");
    print_regname(rn);
    printf(": %016lx\n", *get_cpu_reg((y86_t *)cpu, r++));
   
}

//=======================================================================

y86_register_t decode_execute(  y86_t *cpu , bool *cond , const y86_inst_t *inst ,
                                y86_register_t *valA 
                             )
{
    if (!cpu || !cond || !inst || !valA) {
        cpu->stat = INS;
        cpu->pc = -1;
        return 0;
    }

    y86_register_t valE = 0;
    y86_register_t *ra, *rb, rsp;

    ra = get_cpu_reg(cpu, inst->ra);  
    rb = get_cpu_reg(cpu, inst->rb);
    rsp = cpu->rsp;

    switch(inst->type) {
        case HALT:
            cpu->stat = HLT;
            break;
        case NOP:
            cpu->stat = AOK;
            break;
        case CMOV:
            cpu->stat = AOK;
            *valA = *ra;
            valE = *valA;
            switch(inst->cmov) {
                case RRMOVQ:
                    *cond = true;
                    break;
                case CMOVLE:
                    *cond = (cpu->sf ^ cpu->of)|cpu->zf;
                    break;
                case CMOVL:
                    *cond = cpu->sf ^ cpu->of;
                    break;
                case CMOVE:
                    *cond = cpu->zf;
                    break;
                case CMOVNE:
                    *cond = !cpu->zf;
                    break;
                case CMOVGE:
                    *cond = !(cpu->sf ^ cpu->of);
                    break;
                case CMOVG:
                    *cond = !(cpu->sf ^ cpu->of) & !cpu->zf;
                    break;
                default:
                    cpu->stat = INS;
                    cpu->pc = -1;
                    *cond = false;
                    return 0;
            }
            break;
        case IRMOVQ:
            cpu->stat = AOK;
            valE = inst->value;
            break;
        case RMMOVQ:
            cpu->stat = AOK;
            *valA = *ra;
            valE = *rb + inst->d;
            if (valE >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return 0;
            }
            break;
        case MRMOVQ:
            if (valE >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return 0;
            }
            valE = *rb + inst->d;
            break;
        case OPQ:
            cpu->stat = AOK;
            *valA = *ra;
            long e;
            if (inst->op == ADD) {
                e = (long)*ra + (long)*rb;
                if ((long) *rb > 0)
                    cpu->of = (long)*ra > INT_MAX - (long)*rb ? 1:0;
                if ((long) *rb < 0)
                    cpu->of = (long)*ra < INT_MIN - (long)*rb ? 1:0;
            } else if (inst->op == SUB) {
                e = (long)*rb - (long)*ra;
                if ((long) *rb < 0)
                    cpu->of = (long)*ra > INT_MAX + (long)*rb ? 1:0;
                if ((long) *rb > 0)
                    cpu->of = (long)*ra < INT_MIN + (long)*rb ? 1:0;
            } else if (inst->op == AND) {
                e = (long)*ra & (long)*rb;
                cpu->of = 0;
            } else if (inst->op == XOR){
                e = (long)*ra ^ (long)*rb;
                cpu->of = 0;
            } else {
                cpu->stat = INS;
                cpu->pc = -1;
                return 0;
            }
            cpu->zf = e == 0 ? 1 : 0;
            cpu->sf = e < 0 ? 1 : 0;
            valE = (y86_register_t) e;
            break;
        case JUMP:
            cpu->stat = AOK;
            *cond = false;
            switch(inst->jump) {
                case JMP:
                    *cond = true;
                    break;
                case JLE:
                    if (cpu->zf == 1 || (cpu->sf ^ cpu->of) == 1) {
                        *cond = true;
                    }
                    break;
                case JL:
                    if ((cpu->sf ^ cpu->of) == 1) {
                        *cond = true;
                    }
                    break;
                case JE:
                    if (cpu->zf == 1) {
                        *cond = true;
                    }
                    break;
                case JNE:
                    if (cpu->zf == 0) {
                        *cond = true;
                    }
                    break;
                case JGE:
                    if (!(cpu->sf ^ cpu->of)) {
                        *cond = true;
                    }
                    break;
                case JG:
                    if (!(cpu->sf ^ cpu->of) && !cpu->zf) {
                        *cond = true;
                    }
                    break;
                default:
                    cpu->stat = INS;
                    cpu->pc = -1;
                    *cond = false;
                    return 0;
            }

            break;
        case CALL:
            cpu->stat = AOK;
            valE = rsp - 8;
            if (inst->dest >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return 0;
            }
            break;
        case RET:
            cpu->stat = AOK;
            *valA = rsp;
            valE = rsp + 8;
            break;
        case PUSHQ:
            cpu->stat = AOK;
            *valA = *ra;
            valE = rsp - 8;          
            break;
        case POPQ:
            cpu->stat = AOK;
            *valA = rsp;
            valE = rsp + 8;
            break;
        default:
            cpu->stat = INS;
            cpu->pc = -1;
            return 0;

    }
    return valE;
}

//=======================================================================

void memory_wb_pc(  y86_t *cpu , memory_t memory , bool cond , 
                    const y86_inst_t *inst , y86_register_t  valE , 
                    y86_register_t  valA 
                 )
{
    if (!cpu || !inst) {
        return;
    }
    if (cpu->stat != AOK) {
        cpu->pc = -1;
    }
    
    y86_register_t *ra, *rb;

    ra = get_cpu_reg(cpu, inst->ra);  
    rb = get_cpu_reg(cpu, inst->rb);

    switch(inst->type) {
        case HALT:
            cpu->pc = 0;
            cpu->zf = 0;
            cpu->sf = 0;
            cpu->of = 0;
            break;
        case NOP:
            cpu->pc += inst->size;
            break;
        case CMOV:
            if (cond) {
                *rb = valE;
            } 
            cpu->pc += inst->size;
            break;
        case IRMOVQ:
            *rb = valE;
            cpu->pc += inst->size;
            break;
        case RMMOVQ:
            if (valE >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return;
            }
            *(y86_register_t *)(memory + valE) = valA;
            cpu->pc += inst->size;
            printf("Memory write to 0x%04lx: 0x%lx\n", valE, valA);
            break;
        case MRMOVQ:
            if (valE >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return;
            }
            *ra = *(y86_register_t *)(memory + valE);
            
            cpu->pc += inst->size;
            break;
        case OPQ:
            *rb = valE;
            cpu->pc += inst->size;
            break;
        case JUMP:
            cpu->pc = cond ? inst->dest : cpu->pc + inst->size; 
            break;
        case CALL:
            if (valE >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return;
            }
            *(y86_register_t *)(memory + valE) = cpu->pc + inst->size;
            
            cpu->rsp = valE;
            cpu->pc = inst->dest;
            printf("Memory write to 0x%04lx: 0x%lx\n", valE, *(y86_register_t *)(memory + valE));
            break;
        case RET:
            if (valA >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return;
            }
            cpu->rsp = valE;
            cpu->pc = *(y86_register_t *)(memory+valA);
            break;
        case PUSHQ:
            if (valE >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return;
            }
            *(y86_register_t *)(memory+valE) = valA;
            cpu->rsp = valE;
            cpu->pc += inst->size;
            printf("Memory write to 0x%04lx: 0x%lx\n", valE, valA);
            break;
        case POPQ:
            if (valA >= MEMSIZE) {
                cpu->stat = ADR;
                cpu->pc = -1;
                return;
            }
            cpu->rsp = valE;
            *ra = *(y86_register_t *)(memory+valA);
            cpu->pc += inst->size;
            break;
        default:
            cpu->stat = INS;
            cpu->pc = -1;
            break;
    }
        

}

void print_regname(y86_regname_t reg) {
    
    switch (reg) {
        case RAX:
            printf("%%rax");
            break;
        case RCX:
            printf("%%rcx");
            break;
        case RDX:
            printf("%%rdx");
            break;
        case RBX:
            printf("%%rbx");
            break;
        case RSP:
            printf("%%rsp");
            break;
        case RBP:
            printf("%%rbp");
            break;
        case RSI:
            printf("%%rsi");
            break;
        case RDI:
            printf("%%rdi");
            break;
        case R8:
            printf(" %%r8");
            break;
        case R9:
            printf(" %%r9");
            break;
        case R10:
            printf("%%r10");
            break;
        case R11:
            printf("%%r11");
            break;
        case R12:
            printf("%%r12");
            break;
        case R13:
            printf("%%r13");
            break;
        case R14:
            printf("%%r14");
            break;
        default:
            printf("BADREG");
            break;
    }
}

y86_register_t *get_cpu_reg(y86_t *cpu, y86_rnum_t reg) {
    switch(reg) {
        case 0:
            return &cpu->rax;
        case 1:
            return &cpu->rcx;
        case 2:
            return &cpu->rdx;
        case 3:
            return &cpu->rbx;
        case 4:
            return &cpu->rsp;
        case 5:
            return &cpu->rbp;
        case 6:
            return &cpu->rsi;
        case 7:
            return &cpu->rdi;
        case 8:
            return &cpu->r8;
        case 9:
            return &cpu->r9;
        case 10:
            return &cpu->r10;
        case 11:
            return &cpu->r11;
        case 12:
            return &cpu->r12;
        case 13:
            return &cpu->r13;
        case 14:
            return &cpu->r14;
        default:
            return NULL;
    }

    return NULL;
}

