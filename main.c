#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, char* argv[])
{
    Chunk chunk;
    init_chunk(&chunk);

    int const_idx = add_constant(&chunk, 6.7);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, const_idx, 123);

    write_chunk(&chunk, OP_RETURN, 123);

    disassemble_chunk(&chunk, "test chunk");

    free_chunk(&chunk);
    return 0;
}