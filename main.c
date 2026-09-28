#include <stdio.h>
#include "lexer.h"

int main(int argc, char *argv[]) 
{
    initializeLexer(argv[1]);

    Token token;
    while ((token = getNextToken()).lexeme[0] != '\0')
{
    if (token.type == UNKNOWN)
        continue;

    switch (token.type)
    {
        case KEYWORD:
            printf("Token: %s, Type: KEYWORD\n", token.lexeme);
            break;

        case OPERATOR:
            printf("Token: %s, Type: OPERATOR\n", token.lexeme);
            break;

        case SPECIAL_CHARACTER:
            printf("Token: %s, Type: SPECIAL CHARACTER\n", token.lexeme);
            break;

        case CONSTANT:
            printf("Token: %s, Type: CONSTANT\n", token.lexeme);
            break;

        case IDENTIFIER:
            printf("Token: %s, Type: IDENTIFIER\n", token.lexeme);
            break;
    }
}
return 0;
}

