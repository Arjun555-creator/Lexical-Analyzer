#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static const char* keywords[MAX_KEYWORDS] = {
"int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

static const char* operators = "+-*/%=!<>|&";
static const char* specialCharacters = ",;{}()[]";

//Intialize lexer Function

static FILE *sourcefile;

static int braceCount = 0;
static int parenCount = 0;
static int bracketOpen = 0;
static int bracketLine = 0;
static int currentLine = 1;
static int validationError = 0;

void initializeLexer(const char *filename)
{
    sourcefile = fopen(filename, "r");
    if (sourcefile == NULL)
    {
        printf("Error, Unable to open the file\n");
    }
}
int validateToken(const char *str);
void validateEndOfFile(void);

//Next token function
static int savedChar = -1;
Token getNextToken()
{
    Token token;
    char ch;
    int i = 0;
    token.lexeme[0] = '\0';
    token.type = UNKNOWN;
    while (1)
    {
        if (savedChar != -1)
        {
            ch = savedChar;
            savedChar = -1;
        }
        else
        {
            ch = fgetc(sourcefile);
        }

        if (ch == EOF)
        {
            validateEndOfFile();
            break;
        }

        if (isspace(ch))
        {
            if (ch == '\n')
                currentLine++;
            if (i > 0)
                break;
            continue;
        }
        if (isSpecialCharacter(ch) || strchr(operators, ch) != NULL)
        {
            if (i == 0)
            {
                token.lexeme[i++] = ch;
            }
            else
            {
                savedChar = ch;
            }
            break;
        }

        token.lexeme[i++] = ch;

        if (i >= MAX_TOKEN_SIZE - 1)
            break;
    }

    token.lexeme[i] = '\0';

    if (i > 0)
    {
        if (validateToken(token.lexeme))
        {
            token.type = UNKNOWN;
            return token;
        }
        categorizeToken(&token);
    }
    return token;
} 
//Cateogrize Token function

void categorizeToken(Token *token)
{
    if (isKeyword(token -> lexeme))
    {
        token -> type = KEYWORD;
    }
    else if (isOperator(token -> lexeme))
    {
        token -> type = OPERATOR;
    }
    else if (isSpecialCharacter(token -> lexeme[0]))
    {
        token -> type = SPECIAL_CHARACTER;
    }
    else if (isConstant(token -> lexeme))
    {
        token -> type = CONSTANT;
    }
    else if (isIdentifier(token -> lexeme))
    {
        token -> type = IDENTIFIER;
    }
    else
    {
        token -> type = UNKNOWN;
    }
}

//KEYWORD FUNCTION

int isKeyword(const char *s)
{
    int i;
    for (i = 0; i < MAX_KEYWORDS; i++)
    {
        if (strcmp(s, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

//OPERATOR FUNCTION

int isOperator(const char *s)
{
    return (strchr(operators, s[0]) != NULL);
}

//SPECIAL CHARACTER 

int isSpecialCharacter(char ch)
{
    return (strchr(specialCharacters, ch) != NULL);
}

//CONSTANT CHARACTER

int isConstant(const char *str)
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
    {
        if(!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

//IDENTIFIER 

int isIdentifier(const char *str)
{
    int i;

    if (!isalpha(str[0]) && str[0] != '_')
    {
        return 0;
    }

    for (i = 1; str[i] != '\0'; i++)
    {
        if (!isalnum(str[i]) && str[i] != '_')
        {
            return 0;
        }
    }

    return 1;
}
  
int validateToken (const char *str)
{
    int i;
    validationError = 0;

    //Binary Number
    if (str[0] == '0' && (str[1] == 'b' || str[1] == 'B'))
    {
        if (str[2] == '\0')
        {
            printf("Error: Invalid binary number\n");
            validationError = 1;
        }

        for (i = 2; str[i] != '\0'; i++)
        {
            if (str[i] != '0' && str[i] != '1')
            {
                printf("Error: Invalid binary number\n");
                validationError = 1;
                break;
            }
        }
    }

    //Octal Number
    else if(str[0] == '0' && isdigit(str[1]))
    {
        for (i = 1; str[i] != 0; i++)
        {
            if (str[i] < '0' || str[i] > '7')
            {
                printf("Error : Invalid octal number\n");
                validationError = 1; 
                break;
            }
        }
    }

    //Hexadecimal Number
    else if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        if(str[2] == '\0')
        {
            printf("Error : Invalid Hexadecimal number\n");
            validationError = 1;
        }
        for (i = 2; str[i] != '\0'; i++)
        {
            if (!isxdigit(str[i]))
            {
                printf("Error : Invalid Hexadecimal number\n");
                validationError = 1;
                break;
            }
        }
    }

    // Number before Alphabets
    else if (isdigit(str[0]))
    {
        for (i = 1; str[i] != '\0'; i++)
        {
            if (isalpha(str[i]))
            {
                printf("Error: Invalid identifier/decimal number : '%s'\n", str);
                validationError = 1;
                break;
            }
        }
    }
    
    //Curly Braces
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '{')
        {
            braceCount++;
        }
        else if(str[i] == '}')
        {
            if (braceCount == 0)
            {
                printf("Error : Unmatched closing brace } \n");
                validationError = 1;
            }
            else
            {
                braceCount--;
            }
        }
    }

    //Parentheses
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '(')
        {
            parenCount++;
        }
        else if (str[i] == ')')
        {
            if (parenCount == 0)
            {
                printf("Unmatched closing parentheses ) \n");
                validationError = 1;
            }
            else
            {
                parenCount--;
            }
        }
    }

    //Square brackets
    for (i = 0; str[i] != 0; i++)
    {
        if (str[i] == '[')
        {
            bracketOpen = 1;
            bracketLine = currentLine;
        }
        else if (str[i] == ']')
        {
            if (!bracketOpen)
            {
                printf("Error : Unmatched closing bracket ]\n");
                validationError = 1;
            }
            else if (currentLine != bracketLine)
            {
                printf("Error : Square brackets should be open and close at the same line\n");
                validationError = 1;
                bracketOpen = 0;
            }
            else
            {
                bracketOpen = 0;
            }
        }
    }
    return validationError;
}

    //EOF FUNCTION
void validateEndOfFile()
{
    if (braceCount > 0)
    {
        printf("Error: Unclosed '{'\n");
    }

    if (parenCount > 0)
    {
        printf("Error: Unclosed '('\n");
    }
}

























































