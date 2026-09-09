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

static Token scan_true(const char **current)
{
    Token token;

    token.type = TOKEN_TRUE;
    token.start = *current;

    if ((*current)[0] == 't' &&
        (*current)[1] == 'r' &&
        (*current)[2] == 'u' &&
        (*current)[3] == 'e')
    {
        (*current) += 4;
        token.length = 4;

        return token;
    }

    token.type = TOKEN_ERROR;
    token.length = 1;
    (*current)++;

    return token;
}

static Token scan_false(const char **current)
{
    Token token;

    token.type = TOKEN_FALSE;
    token.start = *current;

    if ((*current)[0] == 'f' &&
        (*current)[1] == 'a' &&
        (*current)[2] == 'l' &&
        (*current)[3] == 's' &&
        (*current)[4] == 'e')
    {
        (*current) += 5;
        token.length = 5;

        return token;
    }

    token.type = TOKEN_ERROR;
    token.length = 1;
    (*current)++;

    return token;
}

static Token scan_null(const char **current)
{
    Token token;

    token.type = TOKEN_NULL;
    token.start = *current;

    if (**current == 'n' &&
        (*current)[1] == 'u' &&
        (*current)[2] == 'l' &&
        (*current)[3] == 'l')
    {
        (*current) += 4;
        token.length = 4;
        return token;
    }

    token.type = TOKEN_ERROR;
    (*current)++;
    token.length = 1;

    return token;
}

Token tokenizer_next(const char **current)
{
    skip_whitespace(current);

    if (isdigit((unsigned char)**current) || **current == '-')
    {
        return scan_number(current);
    }

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

    case 'n':
        return scan_null(current);

    case 't':
        return scan_true(current);

    case 'f':
        return scan_false(current);

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
