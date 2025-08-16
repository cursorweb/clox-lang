#include "chunk.h"
#include "memory.h"

// TODO: why not just malloc inside?
void init_chunk(Chunk* chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
}

void write_chunk(Chunk* chunk, uint8_t byte)
{
    // TODO: why not OpCode?
    if (chunk->capacity < chunk->count + 1)
    {
        int old_capacity = chunk->capacity;
        int new_capacity = GROW_CAPACITY(old_capacity);
        uint8_t* new_code =
            GROW_ARRAY(uint8_t, chunk->code, old_capacity, new_capacity);

        chunk->capacity = new_capacity;
        chunk->code = new_code;
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;
}

void free_chunk(Chunk* chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    init_chunk(chunk);
}