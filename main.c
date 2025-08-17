#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char* argv[])
{
    init_vm();
    Chunk chunk;
    init_chunk(&chunk);

    // -((1.2 + 3.4) / 5.6)
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, add_constant(&chunk, 1.2), 123);

    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, add_constant(&chunk, 3.4), 123);

    write_chunk(&chunk, OP_ADD, 123);

    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, add_constant(&chunk, 5.6), 123);

    write_chunk(&chunk, OP_DIV, 123);

    write_chunk(&chunk, OP_NEGATE, 123);

    write_chunk(&chunk, OP_RETURN, 123);

    disassemble_chunk(&chunk, "test chunk");

    interpret(&chunk);

    free_vm();
    free_chunk(&chunk);
    return 0;
}