#include "vm.h"
#include "common.h"
#include "debug.h"
#include <stdio.h>

VM vm; // global variable :(

static void reset_stack()
{
    // pointer decay, vm.stack (Value[STACK_MAX]) -> vm.stack_top (Value*)
    vm.stack_top = vm.stack;
}

void init_vm()
{
    reset_stack();
}

void free_vm()
{
}

// static makes this function private
static InterpretResult run()
{
// vm.ip++; return *(vm.ip)
// reads an op code
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    while (true)
    {
#ifdef DEBUG_TRACE_EXECUTION
        disassemble_instr(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instr;
        switch (instr = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            print_value(constant);
            printf("\n");
        }
        break;
        case OP_RETURN:
        {
            return INTERPRET_OK;
        }
        break;
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk* chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}
