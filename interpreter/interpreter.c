// This is where you put your VM code.
// I am trying to follow the coding style of the original.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "minivm.h"

#define NUM_REGS   (256)
#define NUM_FUNCS  (256)
#define SIZE_HEAP  (8192)

// Global variable that indicates if the process is running.
static bool is_running = true;

// Custom logging macro
// Prints error in the same way as printf to stderr
// also stops program by settings is_running
#define log_errf(msg, ...)                                      \
    do {                                                        \
        fprintf(stderr, "[error]: " msg "\n", ##__VA_ARGS__);   \
        exit(1);                                                \
    } while (0)

// Print usage and exit
void usageExit(char **argv) {
    fprintf(stderr, "usage: %s <bytecode>\n", argv[0]);
    exit(1);
}

// Validates if heap address is within bounds of actual heap
bool validHeapAddress(uint32_t addr) {
    return (0 <= addr) && (addr < SIZE_HEAP);
}

// Calculate heap address with offsetting
uint32_t getHeapAddr(uint32_t heap, uint32_t offset) {
    if (!validHeapAddress(offset)) {
        log_errf("heap address %p invalid", offset);
    }

    return heap + offset;
}


// Instruction execution semantics

void instr_halt(struct VMContext* ctx, const uint32_t instr) {
    printf("halt\n");           /* debug */

    is_running = false;
}

void instr_load(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx = EXTRACT_B2(instr);
    uint32_t srcRegVal = ctx->r[srcRegIdx].value;

    printf("load r%d r%d\n", destRegIdx, srcRegIdx); /* debug */

    uint32_t addr = getHeapAddr(ctx->heap, srcRegVal);

    uint32_t value = *((uint32_t *) addr);

    printf("VALUE: %d\n", value);

    ctx->r[destRegIdx].value = value;
}

void instr_store(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx = EXTRACT_B2(instr);
    uint32_t destRegVal = ctx->r[destRegIdx].value;
    uint32_t srcRegVal = ctx->r[srcRegIdx].value;

    printf("store r%d r%d\n", destRegIdx, srcRegIdx); /* debug */

    uint32_t addr = getHeapAddr(ctx->heap, destRegVal);

    uint32_t value = (uint32_t) EXTRACT_B3(srcRegVal);

    printf("VALUE: %d\n", value);

    *((uint32_t *) addr) = value;
}

void instr_move(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx = EXTRACT_B2(instr);

    printf("move r%d r%d\n", destRegIdx, srcRegIdx); /* debug */

    ctx->r[destRegIdx].value = ctx->r[srcRegIdx].value;
}

void instr_puti(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t v = EXTRACT_B2(instr);

    uint32_t value = (uint32_t) v;

    printf("puti r%d %d\n", destRegIdx, value); /* debug */

    ctx->r[destRegIdx].value = value;
}

void instr_add(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx1 = EXTRACT_B2(instr);
    uint8_t srcRegIdx2 = EXTRACT_B3(instr);

    printf("add r%d r%d r%d\n", destRegIdx, srcRegIdx1, srcRegIdx2); /* debug */

    ctx->r[destRegIdx].value =
        ctx->r[srcRegIdx1].value +
        ctx->r[srcRegIdx2].value;
}

void instr_sub(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx1 = EXTRACT_B2(instr);
    uint8_t srcRegIdx2 = EXTRACT_B3(instr);

    printf("sub r%d r%d r%d\n", destRegIdx, srcRegIdx1, srcRegIdx2); /* debug */

    ctx->r[destRegIdx].value =
        ctx->r[srcRegIdx1].value -
        ctx->r[srcRegIdx2].value;
}

void instr_gt(struct VMContext* ctx, const uint32_t instr) {}
void instr_ge(struct VMContext* ctx, const uint32_t instr) {}
void instr_eq(struct VMContext* ctx, const uint32_t instr) {}
void instr_ite(struct VMContext* ctx, const uint32_t instr) {}
void instr_jump(struct VMContext* ctx, const uint32_t instr) {}

void instr_puts(struct VMContext* ctx, const uint32_t instr) {
    uint8_t regIdx = EXTRACT_B1(instr);
    uint32_t regVal = ctx->r[regIdx].value;

    printf("puts r%d\n", regIdx); /* debug */

    uint32_t addr = getHeapAddr(ctx->heap, regVal);

    printf("%s", (char *) addr);
}

void instr_gets(struct VMContext* ctx, const uint32_t instr) {
    uint8_t regIdx = EXTRACT_B1(instr);
    uint32_t regVal = ctx->r[regIdx].value;

    char buf[128];
    fgets(buf, 128, stdin);

    uint32_t addr = getHeapAddr(ctx->heap, regVal);

    strcpy(addr, buf);
}

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

void initRegs(Reg *r, uint32_t cnt) {
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
    uint32_t *heap;
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

    r[4].value = UINT_MAX;

    printf("& heap: %p\n", heap);        /* debug */
    printf("# instr: %d\n", codeSize / 4); /* debug */
    printf("running...\n\n");                /* debug */

    while (is_running) {
        printf("pc: 0x%08x\n", *pc); /* debug */

        stepVMContext(&vm, &pc);
    }

    // Debug: print registers
    printf("\n------- regs -------\n");
    for (int i = 0; i < 5; i++) {
        printf("r%d: %u\n", i, vm.r[i].value);
    }
    printf("--------------------\n");

    // Zero indicates normal termination.
    return 0;
}
