#include "tinyjson/tokenizer.h"
#include <ctype.h>

#define JSON_CONTROL_CHAR_MAX 0x1F

static int is_json_whitespace(char c)
{
    return c == ' ' ||
           c == '\t' ||
           c == '\n' ||
           c == '\r';
}

static int is_token_boundary(char c)
{
    return c == '\0' ||
           c == ' ' ||
           c == '\t' ||
           c == '\n' ||
           c == '\r' ||
           c == ',' ||
           c == ']' ||
           c == '}' ||
           c == ':';
}

static int is_valid_escape(char c)
{
    return c == '"' ||
           c == '\\' ||
           c == '/' ||
           c == 'b' ||
           c == 'f' ||
           c == 'n' ||
           c == 'r' ||
           c == 't' ||
           c == 'u';
}

static void skip_whitespace(const char **current)
{
    while (is_json_whitespace(**current))
    {
        (*current)++;
    }
}

static Token scan_string(const char **current)
{
    Token token;

    token.type = TOKEN_STRING;

    (*current)++;

    token.start = *current;

    while (**current != '\0')
    {
        if (**current == '\\')
        {
            (*current)++;

            if (**current == '\0')
            {
                token.type = TOKEN_ERROR;
                token.length = (size_t)(*current - token.start);
                return token;
            }

            if (!is_valid_escape(**current))
            {
                token.type = TOKEN_ERROR;
                token.length = (size_t)(*current - token.start);
                return token;
            }

            if (**current == 'u')
            {
                (*current)++;

                for (int i = 0; i < 4; i++)
                {
                    if (!isxdigit((unsigned char)**current))
                    {
                        token.type = TOKEN_ERROR;
                        token.length = (size_t)(*current - token.start);
                        return token;
                    }

                    (*current)++;
                }

                continue;
            }

            (*current)++;
            continue;
        }

        if (**current == '"')
        {
            break;
        }

        if ((unsigned char)**current <= JSON_CONTROL_CHAR_MAX)
        {
            token.type = TOKEN_ERROR;
            token.length = (size_t)(*current - token.start);
            return token;
        }

        (*current)++;
    }

    token.length = (size_t)(*current - token.start);

    if (**current == '"')
    {
        (*current)++;
    }
    else
    {
        token.type = TOKEN_ERROR;
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

        /* '-' by itself is not a valid number */
        if (!isdigit((unsigned char)**current))
        {
            token.type = TOKEN_ERROR;
            token.length = (size_t)(*current - token.start);
            return token;
        }
    }

    /*
     * Integer part
     *
     * JSON allows:
     *     0
     *     123
     *
     * But NOT:
     *     01
     *     0123
     */
    if (**current == '0')
    {
        (*current)++;

        /* Leading zero */
        if (isdigit((unsigned char)**current))
        {
            token.type = TOKEN_ERROR;
            token.length = (size_t)(*current - token.start);
            return token;
        }
    }
    else
    {
        while (isdigit((unsigned char)**current))
        {
            (*current)++;
        }
    }

    /* Fraction part */
    if (**current == '.')
    {
        (*current)++;

        /* Decimal point must be followed by a digit */
        if (!isdigit((unsigned char)**current))
        {
            token.type = TOKEN_ERROR;
            token.length = (size_t)(*current - token.start);
            return token;
        }

        while (isdigit((unsigned char)**current))
        {
            (*current)++;
        }
    }

    /* Exponent part */
    if (**current == 'e' || **current == 'E')
    {
        (*current)++;

        /* Optional exponent sign */
        if (**current == '+' || **current == '-')
        {
            (*current)++;
        }

        /* e, e+, e- are invalid */
        if (!isdigit((unsigned char)**current))
        {
            token.type = TOKEN_ERROR;
            token.length = (size_t)(*current - token.start);
            return token;
        }

        while (isdigit((unsigned char)**current))
        {
            (*current)++;
        }
    }

    if (!is_token_boundary(**current))
    {
        token.type = TOKEN_ERROR;
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

        if (!is_token_boundary(**current))
        {
            token.type = TOKEN_ERROR;
        }

        token.length = (size_t)(*current - token.start);
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

        if (!is_token_boundary(**current))
        {
            token.type = TOKEN_ERROR;
        }

        token.length = (size_t)(*current - token.start);

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

        if (!is_token_boundary(**current))
        {
            token.type = TOKEN_ERROR;
        }

        token.length = (size_t)(*current - token.start);

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
