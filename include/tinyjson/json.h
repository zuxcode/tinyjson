#ifndef TINYJSON_JSON_H
#define TINYJSON_JSON_H

typedef enum
{
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

typedef struct JsonValue
{
    JsonType type;

    union
    {
        int boolean;
        double number;
        char *string;
    } data;

} JsonValue;

#endif