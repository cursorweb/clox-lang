#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

typedef struct Parser
{
    Token current;
    Token prev;

    bool had_error;
    bool panic_mode;
} Parser;

Parser parser;

// chunks
Chunk* compiling_chunk;

static Chunk* curr_chunk()
{
    return compiling_chunk;
}

// error handling
static void error_at(Token* token, const char* message)
{
    if (parser.panic_mode)
    {
        return;
    }

    parser.panic_mode = true;
    fprintf(stderr, "[line %d] Error ", token->line);

    if (token->type == TOKEN_EOF)
    {
        fprintf(stderr, "at end");
    }
    else
    {
        fprintf(stderr, "at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.had_error = true;
}

static void error(const char* message)
{
    error_at(&parser.prev, message);
}

static void error_at_current(const char* message)
{
    error_at(&parser.current, message);
}

// token consuming
static void advance()
{
    parser.prev = parser.current;
    while (true)
    {
        parser.current = scan_token();

        if (parser.current.type != TOKEN_ERROR)
        {
            break;
        }

        error_at_current(parser.current.start);
    }
}

static void consume(TType type, const char* message)
{
    if (parser.current.type == type)
    {
        // eat the token
        advance();
        return;
    }

    error_at_current(message);
}

// bytes
static void emit_byte(uint8_t byte)
{
    write_chunk(curr_chunk(), byte, parser.prev.line);
}

static void emit_bytes(uint8_t byte1, uint8_t byte2)
{
    emit_byte(byte1);
    emit_byte(byte2);
}

static int make_constant(Value val)
{
    int constant = add_constant(curr_chunk(), val);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emit_constant(Value val)
{
    emit_bytes(OP_CONSTANT, make_constant(val));
}

static void end_compiler()
{
    emit_byte(OP_RETURN);
}

// parsing and compiling
static void expr()
{
}

static void grouping()
{
    expr();
    consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression.");
}

static void number()
{
    // TODO: why not atof?
    double value = strtod(parser.prev.start, NULL);
    emit_constant(value);
}

static void unary()
{
    TType op_type = parser.prev.type;

    // push expr to stack
    expr();

    // push operator to stack (pops previous value, and then pushes it back on)
    switch (op_type)
    {
    case TOKEN_MINUS:
        emit_byte(OP_NEGATE);
        break;
    default:
        // could be a different operator, so the pratt parser try a different
        // method
        return;
    }
}

bool compile(const char* source, Chunk* chunk)
{
    init_scanner(source);
    compiling_chunk = chunk;

    parser.had_error = false;
    parser.panic_mode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expected EOF.");

    end_compiler();
    return !parser.had_error;
}