#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

static void repl()
{
    char line[1024];
    while (true)
    {
        printf("> ");
        if (!fgets(line, sizeof(line) / sizeof(char), stdin))
        {
            printf("\n");
            break;
        }

        interpret(line);
    }
}

static char* read_file(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        perror("Could not open file\n");
        exit(74);
    }

    fseek(file, 0L, SEEK_END); // move pointer to the end of the file
    size_t file_size = ftell(file);
    rewind(file);

    // add + 1: # of items + \0
    char* buffer = malloc(sizeof(char) * (file_size + 1));
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".", path);
        exit(74);
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size)
    {
        fprintf(stderr, "Could not read file \"%s\".", path);
        exit(74);
    }

    buffer[bytes_read] = '\0'; // remember \0 terminates the string in memory

    fclose(file);

    return buffer;
}

static void run_file(const char* path)
{
    char* source = read_file(path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERR)
        exit(65);
    if (result == INTERPRET_RUNTIME_ERR)
        exit(70);
}

int main(int argc, const char* argv[])
{
    init_vm();

    if (argc == 1)
    {
        repl();
    }
    else if (argc == 2)
    {
        run_file(argv[1]);
    }
    else
    {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }

    free_vm();
    return 0;
}