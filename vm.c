#include <stdarg.h>
#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

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
    // for now, the stack you just let C clean up
}

static void runtime_err(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    // this gets the index: (vm.ip - vm.chunk->code) returns 0+, but vm.ip is
    // the *next* instruction
    size_t instr_idx = vm.ip - 1 - vm.chunk->code;
    int line = vm.chunk->lines[instr_idx];
    fprintf(stderr, "[line %d] in script\n", line);
    reset_stack();
}

static Value peek(int dist)
{
    return vm.stack_top[-1 - dist];
}

static bool is_falsey(Value val)
{
    return IS_NIL(val) || (IS_BOOL(val) && !AS_BOOL(val));
}

// static makes this function private
// goes through the bytecode (vm.chunk), and interprets it.
static InterpretResult run()
{
// vm.ip++; return *(vm.ip)
// reads an op code
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
// using a do while loop lets you add semicolon at the end of it.
// b comes first, because last in *first* out!
#define BINARY_OP(value_type, op)                                              \
    do                                                                         \
    {                                                                          \
        if (!IS_NUM(peek(0)) || !IS_NUM(peek(1)))                              \
        {                                                                      \
            runtime_err("Operands must be numbers");                           \
            return INTERPRET_RUNTIME_ERR;                                      \
        }                                                                      \
        double b = AS_NUM(vm.stack_top[-1]);                                   \
        double a = AS_NUM(vm.stack_top[-2]);                                   \
        vm.stack_top[-2] = value_type(a op b);                                 \
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
        case OP_NIL:
            push(NIL_VAL);
            break;
        case OP_TRUE:
            push(BOOL_VAL(true));
            break;
        case OP_FALSE:
            push(BOOL_VAL(false));
            break;
        case OP_NOT:
            vm.stack_top[-1] = BOOL_VAL(is_falsey(vm.stack_top[-1]));
            break;
        case OP_EQUAL:
            Value b = vm.stack_top[-1];
            Value a = vm.stack_top[-2];
            vm.stack_top[-2] = BOOL_VAL(values_equal(a, b));
            vm.stack_top--;
            break;
        case OP_GRTR:
            BINARY_OP(BOOL_VAL, >);
            break;
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OP_ADD:
            BINARY_OP(NUM_VAL, +);
            break;
        case OP_SUB:
            BINARY_OP(NUM_VAL, -);
            break;
        case OP_MULT:
            BINARY_OP(NUM_VAL, *);
            break;
        case OP_DIV:
            BINARY_OP(NUM_VAL, /);
            break;
        case OP_NEGATE:
        {
            if (!IS_NUM(peek(0)))
            {
                runtime_err("'-' can only be used on numbers.");
                return INTERPRET_RUNTIME_ERR;
            }
            // a[b] is same as *(vm.stack_top - 1)
            vm.stack_top[-1] = NUM_VAL(-AS_NUM(vm.stack_top[-1]));
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

// the main function where everything is done:
// compiling, and running
InterpretResult interpret(const char* source)
{
    Chunk chunk;
    init_chunk(&chunk);

    if (!compile(source, &chunk))
    {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    free_chunk(&chunk);
    return result;
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