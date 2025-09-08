#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

// like a private class
typedef struct Scanner
{
    // start is a pointer (aka index) of the start of the lexeme
    const char* start;
    // current - start = length of lexeme
    // note that current is not inclusive! it goes past the lexeme
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

static bool is_digit(char c)
{
    return '0' <= c && c <= '9';
}

static bool is_alpha(char c)
{
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_';
}

static bool at_end()
{
    return *scanner.current == '\0';
}

static bool match(char expected)
{
    if (at_end())
    {
        return false;
    }

    if (*scanner.current == expected)
    {
        scanner.current++;
        return true;
    }

    return false;
}

static char peek()
{
    return *scanner.current;
}

static char peek_next()
{
    if (at_end())
        return '\0';
    return scanner.current[1];
}

static char advance()
{
    scanner.current++;

    // return previous
    return scanner.current[-1];
}

static void skip_whitespace()
{
    while (true)
    {
        char c = peek();
        // if you don't get any of these whitespace, continue to the main loop
        // where we return EOF on '\0'
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            scanner.line++;
            advance();
            break;
        case '/':
            if (peek_next() == '/')
            {
                while (peek() != '\n' && !at_end())
                {
                    advance();
                }
            }
            else
            {
                // don't consume '/'
                return;
            }
            break;
        default:
            return;
        }
    }
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

static TType check_keyword(int start, int length, const char* rest, TType type)
{
    if (
        // makes sure that the whole token is the same length as the keyword
        // for example, "a" "nd" -> start + length -> 1 + 2 == 3
        // an added bonus is that you don't accidentally read past the string
        (scanner.current - scanner.start == start + length) &&
        memcmp(scanner.start + start, rest, length) == 0)
    {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TType identifier_type()
{
    // trie structure
    switch (scanner.start[0])
    {
    // 2 because nd is length 2
    case 'a':
        return check_keyword(1, 2, "nd", TOKEN_AND);
    case 'c':
        return check_keyword(1, 4, "lass", TOKEN_CLASS);
    case 'e':
        return check_keyword(1, 3, "lse", TOKEN_ELSE);
    case 'f':
        // check to make sure the the keyword is *at least* 2 long
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'a':
                return check_keyword(2, 3, "lse", TOKEN_FALSE);
            case 'o':
                return check_keyword(2, 1, "r", TOKEN_FOR);
            case 'u':
                return check_keyword(2, 1, "n", TOKEN_FUN);
            }
        }
        break;
    case 'i':
        return check_keyword(1, 1, "f", TOKEN_IF);
    case 'n':
        return check_keyword(1, 2, "il", TOKEN_NIL);
    case 'o':
        return check_keyword(1, 1, "r", TOKEN_OR);
    case 'p':
        return check_keyword(1, 4, "rint", TOKEN_PRINT);
    case 'r':
        return check_keyword(1, 5, "eturn", TOKEN_RETURN);
    case 's':
        return check_keyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'h':
                return check_keyword(2, 2, "is", TOKEN_THIS);
            case 'r':
                return check_keyword(2, 2, "ue", TOKEN_TRUE);
            }
        }
        break;
    case 'v':
        return check_keyword(1, 2, "ar", TOKEN_VAR);
    case 'w':
        return check_keyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

static Token identifier()
{
    while (is_alpha(peek()) || is_digit(peek()))
    {
        advance();
    }

    return make_token(identifier_type());
}

static Token number()
{
    while (is_digit(peek()))
    {
        advance();
    }

    if (peek() == '.' && is_digit(peek_next()))
    {
        // '.'
        advance();
        while (is_digit(peek()))
        {
            advance();
        }
    }

    return make_token(TOKEN_NUMBER);
}

static Token string()
{
    while (peek() != '"' && !at_end())
    {
        if (peek() == '\n')
        {
            scanner.line++;
        }
        advance();
    }

    if (at_end())
    {
        return err_token("Unterminated string.");
    }

    advance(); // "

    // the lexeme value will be scanner.start -> scanner.end, the string itself!
    return make_token(TOKEN_STRING);
}

Token scan_token()
{
    skip_whitespace();
    scanner.start = scanner.current;

    if (at_end())
    {
        return make_token(TOKEN_EOF);
    }

    char c = advance();

    if (is_alpha(c))
    {
        return identifier();
    }

    if (is_digit(c))
    {
        return number();
    }

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
    case '!':
        return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
        return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
        return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
        return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"':
        return string();
    }

    return err_token("Unexpected character.");
}
