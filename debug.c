#include "debug.h"
#include "value.h"
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

    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
    {
        printf("% 4c ", '|');
    }
    else
    {
        printf("% 4d ", chunk->lines[offset]);
    }

    uint8_t instr = chunk->code[offset];
    int new_offset = 0;

    switch (instr)
    {
    case OP_CONSTANT:
        new_offset = constant_instr("OP_CONSTANT", chunk, offset);
        break;
    case OP_RETURN:
        new_offset = simple_instr("OP_RETURN", offset);
        break;

    default:
        printf("Unknown opcode %d", instr);
        new_offset = offset + 1;
        break;
    }
    printf("\n");

    return new_offset;
}

int constant_instr(const char* name, Chunk* chunk, int offset)
{
    uint8_t const_idx = chunk->code[offset + 1];
    printf("%-16s %4d '", name, const_idx);
    print_value(chunk->constants.values[const_idx]);
    printf("'");

    // OP_CONSTANT (ptr)
    return offset + 2;
}

int simple_instr(const char* name, int offset)
{
    printf("%s\n", name);

    return offset + 1;
}
