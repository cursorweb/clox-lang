#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

typedef struct Parser
{
    Token current;
    Token prev;

    bool had_error;
    bool panic_mode;
} Parser;

typedef enum Precedence
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

// Kind of a union here, for example '-' has both a prefix and an infix
typedef struct ParseRule
{
    // as `-5`
    ParseFn prefix;
    // as `5 + 5`
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Parser parser;

/*** chunks ***/
Chunk* compiling_chunk;

static Chunk* curr_chunk()
{
    return compiling_chunk;
}

/*** error handling ***/
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

/*** token consuming ***/
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

/*** bytes ***/
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

static ParseRule* get_rule(TType type);
static void parse_precedence(Precedence precedence);

/*** tree ***/
static void expr()
{
    // TODO: if PREC_ASSIGNMENT is the lowest, why is there a PREC_NONE?
    parse_precedence(PREC_ASSIGNMENT);
}

static void binary()
{
    TType op_type = parser.prev.type;
    ParseRule* rule = get_rule(op_type);
    // the expr to the right will always have a higher precedence
    // (left-associative)
    parse_precedence((Precedence)(rule->precedence + 1));

    switch (op_type)
    {
    // `!=` <=> `!(==)`
    // `>=` <=> `!(<)`
    // `<=` <=> `!(>)`
    case TOKEN_EQUAL_EQUAL:
        emit_byte(OP_EQUAL);
        break;
    case TOKEN_BANG_EQUAL:
        emit_bytes(OP_EQUAL, OP_NOT);
        break;
    case TOKEN_GREATER:
        emit_byte(OP_GRTR);
        break;
    case TOKEN_GREATER_EQUAL:
        emit_bytes(OP_LESS, OP_NOT);
        break;
    case TOKEN_LESS:
        emit_byte(OP_LESS);
        break;
    case TOKEN_LESS_EQUAL:
        emit_bytes(OP_GRTR, OP_NOT);
        break;

    case TOKEN_PLUS:
        emit_byte(OP_ADD);
        break;
    case TOKEN_MINUS:
        emit_byte(OP_SUB);
        break;
    case TOKEN_STAR:
        emit_byte(OP_MULT);
        break;
    case TOKEN_SLASH:
        emit_byte(OP_DIV);
        break;
    default:
        return; // Unreachable.
    }
}

static void unary()
{
    TType op_type = parser.prev.type;

    // push expr to stack
    parse_precedence(PREC_UNARY);

    // push operator to stack (pops previous value, and then pushes it back on)
    switch (op_type)
    {
    case TOKEN_MINUS:
        emit_byte(OP_NEGATE);
        break;
    case TOKEN_BANG:
        emit_byte(OP_NOT);
        break;
    default:
        // unreachable
        return;
    }
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
    emit_constant(NUM_VAL(value));
}

static void literal()
{
    switch (parser.prev.type)
    {
    case TOKEN_FALSE:
        emit_byte(OP_FALSE);
        break;
    case TOKEN_TRUE:
        emit_byte(OP_TRUE);
        break;
    case TOKEN_NIL:
        emit_byte(OP_NIL);
        break;

    default:
        break;
    }
}

// starts at current token, and parses any expression at given precedence level
// or higher
static void parse_precedence(Precedence precedence)
{
    advance(); // consume the token

    // first token must always be a prefix (-, or a number etc.)
    ParseFn prefix_rule = get_rule(parser.prev.type)->prefix;

    if (prefix_rule == NULL)
    {
        // Invalid prefix expression
        error("Expected expression.");
        return;
    }

    prefix_rule();

    // the token is always changing, so...
    // keep consuming until the token is of lower precedence, (e.g. 5*3+2)
    while (precedence <= get_rule(parser.current.type)->precedence)
    {
        advance();
        ParseFn infix_rule = get_rule(parser.prev.type)->infix;
        infix_rule();
    }
}

bool compile(const char* source, Chunk* chunk)
{
    init_scanner(source);
    compiling_chunk = chunk;

    parser.had_error = false;
    parser.panic_mode = false;

    advance();
    expr();
    consume(TOKEN_EOF, "Expected EOF.");

    end_compiler();

#ifdef DEBUG_PRINT_CODE
    if (!parser.had_error)
    {
        disassemble_chunk(curr_chunk(), "code");
    }
#endif

    return !parser.had_error;
}

/*** parsing and compiling ***/
// prefix (-5), infix (5 - 5), precedence
ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static ParseRule* get_rule(TType type)
{
    // returns a reference to avoid copying
    return &rules[type];
}
