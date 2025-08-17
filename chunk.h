#pragma once

#include "common.h"
#include "value.h"

typedef enum OpCode
{
    OP_CONSTANT,
    OP_ADD,
    OP_SUB,
    OP_MULT,
    OP_DIV,
    OP_NEGATE,
    OP_RETURN
} OpCode;

typedef struct Chunk
{
    int count;
    int capacity;

    // ~vector
    // contains instructions as well as values
    // the * here means array and NOT a reference
    uint8_t* code;
    // parallel 'array' to code.
    int* lines;

    ValueArray constants;
} Chunk;

void init_chunk(Chunk* chunk);
void write_chunk(Chunk* chunk, uint8_t byte, int line);
void free_chunk(Chunk* chunk);

int add_constant(Chunk* chunk, Value value);