#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct Scanner
{
    // start is a pointer (aka index) of the start of the lexeme
    const char* start;
    // current - start = length of lexeme
    // note that current is not exclusive! it goes past the lexeme
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void init_scanner(const char* source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool at_end()
{
    return *scanner.current == '\0';
}

static Token make_token(TType type)
{
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

// create an error token, that has a lexeme of the error
static Token err_token(const char* err)
{
    Token token;
    token.type = TOKEN_ERROR;

    // hack: since start is a const char*,
    // we don't *have* to make it point to the source code
    // we can just inject our own string, but make sure to make
    // token.length the length of our own string.
    token.start = err;
    token.length = (int)strlen(err);
    token.line = scanner.line;

    return token;
}

static char advance()
{
    scanner.current++;

    // return previous
    return scanner.current[-1];
}

Token scan_token()
{
    scanner.start = scanner.current;

    if (at_end())
    {
        return make_token(TOKEN_EOF);
    }

    char c = advance();

    switch (c)
    {
    case '(':
        return make_token(TOKEN_LEFT_PAREN);
    case ')':
        return make_token(TOKEN_RIGHT_PAREN);
    case '{':
        return make_token(TOKEN_LEFT_BRACE);
    case '}':
        return make_token(TOKEN_RIGHT_BRACE);
    case ';':
        return make_token(TOKEN_SEMICOLON);
    case ',':
        return make_token(TOKEN_COMMA);
    case '.':
        return make_token(TOKEN_DOT);
    case '-':
        return make_token(TOKEN_MINUS);
    case '+':
        return make_token(TOKEN_PLUS);
    case '/':
        return make_token(TOKEN_SLASH);
    case '*':
        return make_token(TOKEN_STAR);
    }

    return err_token("Unexpected character.");
}
