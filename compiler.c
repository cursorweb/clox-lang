#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

void compile(const char* source)
{
    init_scanner(source);

    // use this variable so same line tokens are displayed with '|'
    int line = -1;
    while (true)
    {
        Token token = scan_token();
        if (token.line != line)
        {
            printf("%4d ", token.line);
            line = token.line;
        }
        else
        {
            printf("% 4s ", "");
        }

        // the * = token.length, s = token.start. You need it this way because
        // token.start points into the src code, so there's no '\0'
        // (it doesn't make a string)
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF)
        {
            break;
        }
    }
}