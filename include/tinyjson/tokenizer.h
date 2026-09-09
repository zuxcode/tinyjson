

#ifndef TINYJSON_TOKENIZER_H
#define TINYJSON_TOKENIZER_H

#include <stddef.h>

typedef enum
{
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,

    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,

    TOKEN_COLON,
    TOKEN_COMMA,

    TOKEN_STRING,
    TOKEN_NUMBER,

    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,

    TOKEN_EOF,
    TOKEN_ERROR

} TokenType;

typedef struct
{
    TokenType type;
    const char *start;
    size_t length;
} Token;

Token tokenizer_next(const char **current);

const char *token_type_name(TokenType type);

#endif