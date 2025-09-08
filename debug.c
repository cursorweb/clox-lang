#include <stdio.h>

#include "debug.h"
#include "value.h"

static int constant_instr(const char* name, Chunk* chunk, int offset)
{
    uint8_t const_idx = chunk->code[offset + 1];
    printf("%-16s %4d '", name, const_idx);
    print_value(chunk->constants.values[const_idx]);
    printf("'\n");

    // OP_CONSTANT (ptr)
    return offset + 2;
}

static int simple_instr(const char* name, int offset)
{
    printf("%s\n", name);

    return offset + 1;
}

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

    switch (instr)
    {
    case OP_CONSTANT:
        return constant_instr("OP_CONSTANT", chunk, offset);

    case OP_NIL:
        return simple_instr("OP_NIL", offset);
    case OP_TRUE:
        return simple_instr("OP_TRUE", offset);
    case OP_FALSE:
        return simple_instr("OP_FALSE", offset);

    case OP_NOT:
        return simple_instr("OP_NOT", offset);

    case OP_EQUAL:
        return simple_instr("OP_EQUAL", offset);
    case OP_GRTR:
        return simple_instr("OP_GRTR", offset);
    case OP_LESS:
        return simple_instr("OP_LESS", offset);

    case OP_ADD:
        return simple_instr("OP_ADD", offset);
    case OP_SUB:
        return simple_instr("OP_SUB", offset);
    case OP_MULT:
        return simple_instr("OP_MULT", offset);
    case OP_DIV:
        return simple_instr("OP_DIV", offset);

    case OP_POW:
        return simple_instr("OP_POW", offset);

    case OP_NEGATE:
        return simple_instr("OP_NEGATE", offset);

    case OP_RETURN:
        return simple_instr("OP_RETURN", offset);

    default:
        printf("Unknown opcode %d\n", instr);
        return offset + 1;
    }
}
