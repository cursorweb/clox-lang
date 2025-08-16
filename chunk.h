#pragma once

#include "common.h"

typedef enum OpCode
{
    OP_RETURN
} OpCode;

typedef struct Chunk
{
    int count;
    int capacity;

    // contains instructions as well as code
    uint8_t* code;
} Chunk;

void init_chunk(Chunk* chunk);
void write_chunk(Chunk* chunk, uint8_t byte);
void free_chunk(Chunk* chunk);