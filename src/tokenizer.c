#include "tinyjson/tokenizer.h"
#include <ctype.h>

static void skip_whitespace(const char **current)
{
    while (**current == ' ' ||
           **current == '\n' ||
           **current == '\r' ||
           **current == '\t')
    {
        (*current)++;
    }
}

static Token scan_string(const char **current)
{
    Token token;

    token.type = TOKEN_STRING;

    /* Skip opening quote */
    (*current)++;

    token.start = *current;

    while (**current != '"' && **current != '\0')
    {
        (*current)++;
    }

    token.length = (size_t)(*current - token.start);

    /* Skip closing quote */
    if (**current == '"')
    {
        (*current)++;
    }

    return token;
}

// static Token scan_number(const char **current)
// {
//     Token token;

//     token.type = TOKEN_NUMBER;
//     token.start = *current;

//     while (isdigit((unsigned char)**current))
//     {
//         (*current)++;
//     }

//     if (**current == '.')
//     {
//         (*current)++;

//         while (isdigit((unsigned char)**current))
//         {
//             (*current)++;
//         }
//     }

//     token.length = (size_t)(*current - token.start);

//     return token;
// }

static Token scan_number(const char **current)
{
    Token token;

    token.type = TOKEN_NUMBER;
    token.start = *current;

    /* Optional minus */
    if (**current == '-')
    {
        (*current)++;
    }

    /* Integer part */
    while (isdigit((unsigned char)**current))
    {
        (*current)++;
    }

    /* Fraction part */
    if (**current == '.')
    {
        (*current)++;

        while (isdigit((unsigned char)**current))
        {
            (*current)++;
        }
    }

    /* Exponent part */
    if (**current == 'e' || **current == 'E')
    {
        (*current)++;

        if (**current == '+' || **current == '-')
        {
            (*current)++;
        }

        while (isdigit((unsigned char)**current))
        {
            (*current)++;
        }
    }

    token.length = (size_t)(*current - token.start);

    return token;
}

Token tokenizer_next(const char **current)
{
    skip_whitespace(current);

    Token token;

    token.start = *current;
    token.length = 1;

    switch (**current)
    {
    case '{':
        token.type = TOKEN_LEFT_BRACE;
        (*current)++;
        return token;

    case '}':
        token.type = TOKEN_RIGHT_BRACE;
        (*current)++;
        return token;

    case '[':
        token.type = TOKEN_LEFT_BRACKET;
        (*current)++;
        return token;

    case ']':
        token.type = TOKEN_RIGHT_BRACKET;
        (*current)++;
        return token;

    case ':':
        token.type = TOKEN_COLON;
        (*current)++;
        return token;

    case ',':
        token.type = TOKEN_COMMA;
        (*current)++;
        return token;

    case '"':
        return scan_string(current);

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return scan_number(current);

    case '\0':
        token.type = TOKEN_EOF;
        token.length = 0;
        return token;

    default:
        token.type = TOKEN_ERROR;
        (*current)++;
        return token;
    }
}

const char *token_type_name(TokenType type)
{
    switch (type)
    {
    case TOKEN_LEFT_BRACE:
        return "TOKEN_LEFT_BRACE";

    case TOKEN_RIGHT_BRACE:
        return "TOKEN_RIGHT_BRACE";

    case TOKEN_LEFT_BRACKET:
        return "TOKEN_LEFT_BRACKET";

    case TOKEN_RIGHT_BRACKET:
        return "TOKEN_RIGHT_BRACKET";

    case TOKEN_COLON:
        return "TOKEN_COLON";

    case TOKEN_COMMA:
        return "TOKEN_COMMA";

    case TOKEN_STRING:
        return "TOKEN_STRING";

    case TOKEN_NUMBER:
        return "TOKEN_NUMBER";

    case TOKEN_TRUE:
        return "TOKEN_TRUE";

    case TOKEN_FALSE:
        return "TOKEN_FALSE";

    case TOKEN_NULL:
        return "TOKEN_NULL";

    case TOKEN_EOF:
        return "TOKEN_EOF";

    case TOKEN_ERROR:
        return "TOKEN_ERROR";

    default:
        return "UNKNOWN_TOKEN";
    }
}
