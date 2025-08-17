#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

#include <windows.h>

#include <profileapi.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    init_vm();
    Chunk chunk;
    init_chunk(&chunk);

    write_chunk(&chunk, OP_CONSTANT, 123);
    int constant = add_constant(&chunk, 0.1);
    write_chunk(&chunk, constant, 123);

    for (int i = 0; i < 1'000'000; i++)
    {
        // write_chunk(&chunk, OP_CONSTANT, 123);
        // constant = add_constant(&chunk, 0.1);
        // write_chunk(&chunk, constant, 123);

        write_chunk(&chunk, OP_NEGATE, 123);
    }
    write_chunk(&chunk, OP_RETURN, 123);

    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    // code
    interpret(&chunk);
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

    //
    // We now have the elapsed number of ticks, along with the
    // number of ticks-per-second. We use these values
    // to convert to the number of elapsed microseconds.
    // To guard against loss-of-precision, we convert
    // to microseconds *before* dividing by ticks-per-second.
    //

    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

    // "fast" - 2274
    // "slow" *= 2400
    printf("Time Used: %lld\n", ElapsedMicroseconds.QuadPart);

    free_vm();
    free_chunk(&chunk);
    return 0;
}