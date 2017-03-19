//-----------------------------------------------------------------------------
// minivm.c -- The basic definitions for the vm.
// Copyright (C) Philip Conrad 4/30/2013 @ 3:56 PM -- MIT License
//
//-----------------------------------------------------------------------------

#include "minivm.h"


//---------------------------------------------------------
// FUNCTION IMPLEMENTATIONS:


// Defers decoding of register args to the called function.
// dispatch :: VMContext -> uint32_t -> Effect()
void dispatch(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t i = EXTRACT_B0(instr);
    (*ctx->funtable[i])(ctx, instr);
}


// Initializes a VMContext in-place.
// initVMContext :: VMContext -> uint32_t -> uint32_t -> [Reg] -> [FunPtr] -> Effect()
void initVMContext(struct VMContext* ctx,
                     const uint32_t* bytecode,
                      const uint32_t codeSize,
                      const uint32_t numRegs,
                      const uint32_t numFuns,
                      const uint32_t sizeHeap,
                                Reg* registers,
                             FunPtr* funtable,
                           uint32_t* heap) {
    ctx->bytecode   = bytecode;
    ctx->codeSize   = codeSize;
    ctx->pc         = 0;
    ctx->numRegs    = numRegs;
    ctx->numFuns    = numFuns;
    ctx->sizeHeap   = sizeHeap;
    ctx->r          = registers;
    ctx->funtable   = funtable;
    ctx->heap       = heap;
}


// Reads an instruction, executes it, then steps to the next instruction.
// stepVMContext :: VMContext -> uint32_t** -> Effect()
void stepVMContext(struct VMContext* ctx) {
    // Read a 32-bit bytecode instruction.
    uint32_t instr = ctx->bytecode[ctx->pc];

    // Dispatch to an opcode-handler.
    dispatch(ctx, instr);

    // Increment to next instruction.
    ctx->pc++;
}
