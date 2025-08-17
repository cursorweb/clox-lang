#pragma once

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct VM
{
    // why a reference and not just own it?
    Chunk* chunk;
    // loc of current running instruction (instruction pointer)
    // not an int because then you'd have to do `chunk->code[ip]` which is
    // slower
    // always points to the *next* instruction (not executed yet)
    uint8_t* ip;

    Value stack[STACK_MAX];
    // stack_top points past the stack, stack_top == len
    Value* stack_top;
} VM;

typedef enum InterpretResult
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERR,
    INTERPRET_RUNTIME_ERR,
} InterpretResult;

void init_vm();
void free_vm();

InterpretResult interpret(Chunk* chunk);