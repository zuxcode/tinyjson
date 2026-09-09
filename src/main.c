
#include <stdio.h>

#include "tinyjson/tokenizer.h"

int main(void)
{
    const char *json = "{\"temperature\": -28.5.7}";
    const char *current = json;

    Token token;

    do
    {
        token = tokenizer_next(&current);

        // printf("Token type raw: %d\n", token.type);
        // printf("Token type: %s\n",
        //        token_type_name(token.type));
        printf("Token text: %.*s\n", (int)token.length, token.start);

    } while (token.type != TOKEN_EOF);

    return 0;
}