#include "debug.h"
#include <stdio.h>

void disassemble_chunk(Chunk* chunk, const char* name)
{
    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;)
    {
        offset = disassemble_instr(chunk, offset);
    }
}

int disassemble_instr(Chunk* chunk, int offset)
{
    printf("%04d ", offset);

    uint8_t instr = chunk->code[offset];
    int new_offset = 0;

    switch (instr)
    {
    case OP_RETURN:
        new_offset = simple_instruction("OP_RETURN", offset);
        break;

    default:
        printf("Unknown opcode %d", instr);
        new_offset = offset + 1;
        break;
    }
    printf("\n");

    return new_offset;
}

int simple_instruction(const char* name, int offset)
{
    printf("%s\n", name);

    return offset + 1;
}