// This is where you put your VM code.
// I am trying to follow the coding style of the original.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <.h>
#include "minivm.h"

#define NUM_REGS   (256)
#define NUM_FUNCS  (256)
#define SIZE_HEAP  (8192)

// Custom logging macro
#ifdef DEBUG
#define debugf(msg, ...) printf("[debug]: " msg, ##__VA_ARGS__);
#else
#define debugf(msg, ...)
#endif

// Custom error macro
// Prints error in the same way as printf to stderr
// also stops program by settings is_running
#define log_errf(msg, ...)                                      \
    do {                                                        \
        fprintf(stderr, "[error]: " msg, ##__VA_ARGS__);        \
        exit(1);                                                \
    } while (0)

// Global variable that indicates if the process is running.
static bool is_running = true;

// Print usage and exit
void usageExit(char **argv) {
    fprintf(stderr, "usage: %s <bytecode>\n", argv[0]);
    exit(1);
}

// Validates if instruction offset is valid
void validateOffset(struct VMContext *ctx, uint32_t offset) {
    if (offset >= ctx->codeSize) {
        log_errf("offset %d is invalid %d\n", offset, ctx->codeSize);
    }
}

// Validates if heap address is within bounds of actual heap
void validateHeapAddress(struct VMContext *ctx, uint32_t offset) {
    if (offset >= ctx->sizeHeap) {
        log_errf("heap address 0x%u invalid\n", offset);
    }
}

// Calculate heap address with offsetting
uint8_t *getHeapAddr(struct VMContext *ctx, uint32_t offset) {
    validateHeapAddress(ctx, offset);

    return ctx->heap + offset;
}


// Instruction execution semantics

void instrHalt(struct VMContext* ctx, const uint32_t instr) {
    (void) ctx;
    (void) instr;

    debugf("halt\n");           /* debug */

    is_running = false;
}

void instrLoad(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx = EXTRACT_B2(instr);
    uint32_t srcRegVal = ctx->r[srcRegIdx].value;

    debugf("load r%d r%d\n", destRegIdx, srcRegIdx); /* debug */

    uint8_t *addr = getHeapAddr(ctx, srcRegVal);

    // Extend to four bytes
    uint32_t value = 0 | *addr;

    ctx->r[destRegIdx].value = value;
}

void instrStore(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx = EXTRACT_B2(instr);
    uint32_t destRegVal = ctx->r[destRegIdx].value;
    uint32_t srcRegVal = ctx->r[srcRegIdx].value;

    debugf("store r%d r%d\n", destRegIdx, srcRegIdx); /* debug */

    uint8_t *addr = getHeapAddr(ctx, destRegVal);

    // Extract lower byte
    uint8_t value = EXTRACT_B0(srcRegVal);

    *addr = value;
}

void instrMove(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx = EXTRACT_B2(instr);

    debugf("move r%d r%d\n", destRegIdx, srcRegIdx); /* debug */

    ctx->r[destRegIdx].value = ctx->r[srcRegIdx].value;
}

void instrPuti(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t v = EXTRACT_B2(instr);

    // Extend to four bytes
    uint32_t value = (uint32_t) v;

    debugf("puti r%d %d\n", destRegIdx, value); /* debug */

    ctx->r[destRegIdx].value = value;
}

void instrAdd(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx1 = EXTRACT_B2(instr);
    uint8_t srcRegIdx2 = EXTRACT_B3(instr);

    debugf("add r%d r%d r%d\n", destRegIdx, srcRegIdx1, srcRegIdx2); /* debug */

    ctx->r[destRegIdx].value =
        ctx->r[srcRegIdx1].value +
        ctx->r[srcRegIdx2].value;
}

void instrSub(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx1 = EXTRACT_B2(instr);
    uint8_t srcRegIdx2 = EXTRACT_B3(instr);

    debugf("sub r%d r%d r%d\n", destRegIdx, srcRegIdx1, srcRegIdx2); /* debug */

    ctx->r[destRegIdx].value =
        ctx->r[srcRegIdx1].value -
        ctx->r[srcRegIdx2].value;
}

void instrGt(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx1 = EXTRACT_B2(instr);
    uint8_t srcRegIdx2 = EXTRACT_B3(instr);

    debugf("gt r%d r%d r%d\n", destRegIdx, srcRegIdx1, srcRegIdx2); /* debug */

    ctx->r[destRegIdx].value =
        (ctx->r[srcRegIdx1].value > ctx->r[srcRegIdx2].value ? 1 : 0);
}

void instrGe(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx1 = EXTRACT_B2(instr);
    uint8_t srcRegIdx2 = EXTRACT_B3(instr);

    debugf("ge r%d r%d r%d\n", destRegIdx, srcRegIdx1, srcRegIdx2); /* debug */

    ctx->r[destRegIdx].value =
        (ctx->r[srcRegIdx1].value >= ctx->r[srcRegIdx2].value ? 1 : 0);
}

void instrEq(struct VMContext* ctx, const uint32_t instr) {
    uint8_t destRegIdx = EXTRACT_B1(instr);
    uint8_t srcRegIdx1 = EXTRACT_B2(instr);
    uint8_t srcRegIdx2 = EXTRACT_B3(instr);

    debugf("eq r%d r%d r%d\n", destRegIdx, srcRegIdx1, srcRegIdx2); /* debug */

    ctx->r[destRegIdx].value =
        (ctx->r[srcRegIdx1].value == ctx->r[srcRegIdx2].value ? 1 : 0);
}

void instrIte(struct VMContext* ctx, const uint32_t instr) {
    uint8_t srcRegIdx = EXTRACT_B1(instr);
    uint8_t offsetA = EXTRACT_B2(instr);
    uint8_t offsetB = EXTRACT_B3(instr);

    debugf("ite r%d %d %d\n", srcRegIdx, offsetA, offsetB); /* debug */

    // Choose destination instruction
    int offset = (ctx->r[srcRegIdx].value > 0) ? offsetA : offsetB;

    validateOffset(ctx, offset);

    // Set program counter to given offset
    // (minus one to compensate for pc increment)
    ctx->pc = offset - 1;
}

void instrJump(struct VMContext* ctx, const uint32_t instr) {
    uint8_t offset = EXTRACT_B2(instr);

    debugf("jump %d\n", offset); /* debug */

    // Set program counter to given offset
    // (minus one to compensate for pc increment)
    ctx->pc = offset - 1;
}

void instrPuts(struct VMContext* ctx, const uint32_t instr) {
    uint8_t regIdx = EXTRACT_B1(instr);
    uint32_t regVal = ctx->r[regIdx].value;

    debugf("puts r%d\n", regIdx); /* debug */

    uint8_t *addr = getHeapAddr(ctx, regVal);

    printf("%s", (char *) addr);
}

void instrGets(struct VMContext* ctx, const uint32_t instr) {
    uint8_t regIdx = EXTRACT_B1(instr);
    uint32_t regVal = ctx->r[regIdx].value;

    // Input from stdin
    char buf[128];
    fgets(buf, 128, stdin);

    // Remove trailing new line char
    char *posNewline;
    if ((posNewline = strchr(buf, '\n')) != NULL) {
        *posNewline = '\0';
    }

    uint8_t *addr = getHeapAddr(ctx, regVal);

    strcpy((char *) addr, buf);
}

void instrInvalid(struct VMContext* ctx, const uint32_t instr) {
    (void) ctx;
    uint8_t opcode = EXTRACT_B0(instr);

    log_errf("[error]: unknown opcode 0x%02x\n", opcode);
}

// Initialize function table
void initFuncs(FunPtr *f, uint32_t cnt) {
    uint32_t i;

    // Set all functions to invalid
    for (i = 0; i < cnt; i++) {
        f[i] = instrInvalid;
    }

    // Set function pointers in function table
    f[0x00] = instrHalt;
    f[0x10] = instrLoad;
    f[0x20] = instrStore;
    f[0x30] = instrMove;
    f[0x40] = instrPuti;
    f[0x50] = instrAdd;
    f[0x60] = instrSub;
    f[0x70] = instrGt;
    f[0x80] = instrGe;
    f[0x90] = instrEq;
    f[0xa0] = instrIte;
    f[0xb0] = instrJump;
    f[0xc0] = instrPuts;
    f[0xd0] = instrGets;
}

void initRegs(Reg *r, uint32_t cnt) {
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        r[i].type = 0;
        r[i].value = 0;
    }
}

int main(int argc, char **argv) {
    VMContext vm;
    Reg r[NUM_REGS];
    FunPtr f[NUM_FUNCS];
    uint8_t *heap;
    FILE *binaryFile;
    uint32_t *bytecode;
    int codeSize = 0;

    // There should be at least one argument.
    if (argc < 2) usageExit(argv);

    // Load bytecode file
    binaryFile = fopen(argv[1], "rb");
    if (binaryFile == NULL) {
        perror("fopen");
        return 1;
    }

    // Get code size (number of bytes)
    fseek(binaryFile, 0, SEEK_END); /* jump to EOF */
    codeSize = ftell(binaryFile);   /* get offset of file pointer */
    rewind(binaryFile);             /* move pointer to start of file */

    // Allocate and read code
    bytecode = malloc(codeSize);      /* allocate bytes according to code size */
    fread(bytecode, codeSize, 1, binaryFile); /* read code */
    fclose(binaryFile);

    // Initialize registers.
    initRegs(r, NUM_REGS);
    // Initialize interpretation functions.
    initFuncs(f, NUM_FUNCS);
    // Initialize heap
    heap = malloc(SIZE_HEAP);
    // Initialize VM context.
    initVMContext(&vm,
                  bytecode,
                  codeSize,
                  NUM_REGS,
                  NUM_FUNCS,
                  SIZE_HEAP,
                  r, f, heap);

    debugf("& heap: %p\n", heap);        /* debug */
    debugf("# instr: %d\n", codeSize / 4); /* debug */
    debugf("running...\n\n");                /* debug */

    while (is_running) {
        debugf("pc: %03d -> 0x%08x\n", vm.pc, vm.bytecode[vm.pc]); /* debug */

        stepVMContext(&vm);

        // Debug: print registers
p        debugf("------- regs -------\n");
        for (int i = 0; i < 6; i++) {
            debugf("r%d: %u\n", i, vm.r[i].value);
        }
        debugf("--------------------\n");
    }

    // Free memory
    free(heap);
    free(bytecode);

    // Zero indicates normal termination.
    return 0;
}
