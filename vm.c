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
// using a do while loop lets you add semicolon at the end of it.
// b comes first, because last in *first* out!
#define BINARY_OP(op)                                                          \
    do                                                                         \
    {                                                                          \
        double b = vm.stack_top[-1];                                           \
        double a = vm.stack_top[-2];                                           \
        vm.stack_top[-2] = a op b;                                             \
        vm.stack_top--;                                                        \
    } while (false)

    while (true)
    {
#ifdef DEBUG_TRACE_EXECUTION
        printf("% 10s", "");
        for (Value* slot = vm.stack; slot < vm.stack_top; slot++)
        {
            printf("[ ");
            print_value(*slot);
            printf(" ]");
        }
        printf("\n");

        disassemble_instr(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instr;
        switch (instr = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            push(constant);
        }
        break;
        case OP_ADD:
            BINARY_OP(+);
            break;
        case OP_SUB:
            BINARY_OP(-);
            break;
        case OP_MULT:
            BINARY_OP(*);
            break;
        case OP_DIV:
            BINARY_OP(/);
            break;
        case OP_NEGATE:
        {
            // a[b] is same as *(vm.stack_top - 1)
            vm.stack_top[-1] = -vm.stack_top[-1];
        }
        break;
        case OP_RETURN:
        {
            print_value(pop());
            printf("\n");
            return INTERPRET_OK;
        }
        break;
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(Chunk* chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

void push(Value value)
{
    *vm.stack_top = value;
    vm.stack_top++;
}

Value pop()
{
    vm.stack_top--;
    return *vm.stack_top;
}