// This is where you put your VM code.
// I am trying to follow the coding style of the original.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minivm.h"

#define NUM_REGS   (256)
#define NUM_FUNCS  (256)
#define SIZE_HEAP  (8192)

// Global variable that indicates if the process is running.
static bool is_running = true;

// Print usage and exit
void usageExit(char **argv) {
    fprintf(stderr, "usage: %s <bytecode>\n", argv[0]);
    exit(1);
}

// Instruction execution semantics

void instr_halt(struct VMContext* ctx, const uint32_t instr)
{
    printf("halt\n");             /* debug */

    is_running = false;
}

void instr_load(struct VMContext* ctx, const uint32_t instr) {}
void instr_store(struct VMContext* ctx, const uint32_t instr) {}
void instr_move(struct VMContext* ctx, const uint32_t instr) {}
void instr_puti(struct VMContext* ctx, const uint32_t instr){}
void instr_add(struct VMContext* ctx, const uint32_t instr) {}
void instr_sub(struct VMContext* ctx, const uint32_t instr) {}
void instr_gt(struct VMContext* ctx, const uint32_t instr) {}
void instr_ge(struct VMContext* ctx, const uint32_t instr) {}
void instr_eq(struct VMContext* ctx, const uint32_t instr) {}
void instr_ite(struct VMContext* ctx, const uint32_t instr) {}
void instr_jump(struct VMContext* ctx, const uint32_t instr) {}
void instr_puts(struct VMContext* ctx, const uint32_t instr) {}
void instr_gets(struct VMContext* ctx, const uint32_t instr) {}

// Initialize function table
void initFuncs(FunPtr *f, uint32_t cnt) {
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        f[i] = NULL;
    }

    // Set function pointers in function table
    f[0x00] = instr_halt;
    f[0x10] = instr_load;
    f[0x20] = instr_store;
    f[0x30] = instr_move;
    f[0x40] = instr_puti;
    f[0x50] = instr_add;
    f[0x60] = instr_sub;
    f[0x70] = instr_gt;
    f[0x80] = instr_ge;
    f[0x90] = instr_eq;
    f[0xa0] = instr_ite;
    f[0xb0] = instr_jump;
    f[0xc0] = instr_puts;
    f[0xd0] = instr_gets;
}

void initRegs(Reg *r, uint32_t cnt)
{
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        r[i].type = 0;
        r[i].value = 0;
    }
}

int main(int argc, char** argv) {
    VMContext vm;
    Reg r[NUM_REGS];
    FunPtr f[NUM_FUNCS];
    uint32_t* heap;
    FILE* bytecode;
    uint32_t *pc;
    int codeSize = 0;

    // There should be at least one argument.
    if (argc < 2) usageExit(argv);

    // Initialize registers.
    initRegs(r, NUM_REGS);
    // Initialize interpretation functions.
    initFuncs(f, NUM_FUNCS);
    // Initialize heap
    heap = malloc(SIZE_HEAP);
    // Initialize VM context.
    initVMContext(&vm, NUM_REGS, NUM_FUNCS, SIZE_HEAP, r, f, heap);

    // Load bytecode file
    bytecode = fopen(argv[1], "rb");
    if (bytecode == NULL) {
        perror("fopen");
        return 1;
    }

    // Get code size (number of bytes)
    fseek(bytecode, 0, SEEK_END); /* jump to EOF */
    codeSize = ftell(bytecode);   /* get offset of file pointer */
    rewind(bytecode);             /* move pointer to start of file */

    // Allocate and read code
    pc = malloc(codeSize);      /* allocate bytes according to code size */
    fread(pc, codeSize, 1, bytecode); /* read code */
    fclose(bytecode);

    printf("& heap: %p\n", heap);        /* debug */
    printf("# instr: %d\n", codeSize / 4); /* debug */
    printf("running...\n");                /* debug */

    while (is_running) {
        printf("pc: 0x%08x\n", *pc); /* debug */

        stepVMContext(&vm, &pc);
    }
    printf("done...\n");        /* debug */

    // Free heap memory
    free(heap);

    // Zero indicates normal termination.
    return 0;
}
