# tinyjson Roadmap

## 🎯 Version 1 — Primitive JSON Parser

**Goal:** Parse valid JSON containing only primitive values.

Supported:

```text
null
true
false
numbers
strings
```

Not supported yet:

```text
arrays []
objects {}
```

### Milestone 1 — Project foundation

```text
tinyjson/
├── CMakeLists.txt
├── Makefile
├── build.sh
├── include/
│   └── tinyjson/
├── src/
└── tests/
```

Tasks:

* [ ] Set up CMake
* [ ] Set up Makefile
* [ ] Set up build script
* [ ] Set up test framework/simple test runner
* [ ] Establish `include/` and `src/` structure

---

## Milestone 2 — Character Scanner

Understand and implement the basic JSON scanner.

Example:

```c
const char *json = "{\"temperature\": 28.5}";
const char *current = json;
```

Tasks:

* [ ] Walk through characters
* [ ] Detect `'\0'`
* [ ] Advance pointer
* [ ] Read current character
* [ ] Skip whitespace

You'll have:

```c
skip_whitespace()
```

---

## Milestone 3 — Tokenizer

Turn characters into tokens.

Your `tokenizer.h` will contain:

```c
TokenType
Token
tokenizer_next()
```

Tasks:

* [ ] `{`
* [ ] `}`
* [ ] `[`
* [ ] `]`
* [ ] `:`
* [ ] `,`
* [ ] strings
* [ ] numbers
* [ ] `true`
* [ ] `false`
* [ ] `null`
* [ ] EOF
* [ ] error

For v1, arrays and objects can be **recognized as tokens** but the parser does not need to support them yet.

Example:

```text
"hello"
   ↓
TOKEN_STRING

123.45
   ↓
TOKEN_NUMBER

true
   ↓
TOKEN_TRUE

null
   ↓
TOKEN_NULL
```

---

# Milestone 4 — String Parsing

Implement JSON string handling.

Examples:

```json
"hello"
```

```json
"hello world"
```

Eventually:

```json
"hello\nworld"
```

Tasks:

* [ ] Find opening `"`
* [ ] Find closing `"`
* [ ] Calculate string length
* [ ] Handle escaped quotes
* [ ] Handle basic escape sequences
* [ ] Detect unterminated strings

---

# Milestone 5 — Number Parsing

Support JSON numbers.

Examples:

```text
0
10
-10
3.14
-42.5
1e10
-2.5e-3
```

Tasks:

* [ ] Positive integers
* [ ] Negative numbers
* [ ] Decimal numbers
* [ ] Exponents
* [ ] Reject invalid numbers

Eventually the tokenizer should produce:

```c
Token token;
```

with:

```c
token.type == TOKEN_NUMBER
```

and the parser can convert the token to:

```c
double
```

---

# Milestone 6 — Literals

Implement:

```text
true
false
null
```

For example:

```text
true
 ↓
TOKEN_TRUE
```

```text
false
 ↓
TOKEN_FALSE
```

```text
null
 ↓
TOKEN_NULL
```

Also detect invalid input:

```text
tru
fals
nul
```

---

# Milestone 7 — JsonValue

Create the internal representation for primitive JSON values.

Your current design is a good starting point:

```c
typedef enum
{
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;
```

For v1, the important types are:

```text
JSON_NULL
JSON_BOOL
JSON_NUMBER
JSON_STRING
```

Then:

```c
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
```

We can leave `JSON_ARRAY` and `JSON_OBJECT` in the enum as **reserved for v2**, or remove them from the v1 API until v2.

---

# Milestone 8 — Parser

Now connect everything:

```text
JSON
 ↓
Tokenizer
 ↓
Tokens
 ↓
Parser
 ↓
JsonValue
```

The parser should understand:

```text
null    → JsonValue(JSON_NULL)
true    → JsonValue(JSON_BOOL)
false   → JsonValue(JSON_BOOL)
123.4   → JsonValue(JSON_NUMBER)
"hello" → JsonValue(JSON_STRING)
```

Implement:

```c
parse_value()
```

This becomes the central function.

Conceptually:

```c
switch (token.type)
{
    case TOKEN_NULL:
        // create JSON_NULL

    case TOKEN_TRUE:
    case TOKEN_FALSE:
        // create JSON_BOOL

    case TOKEN_NUMBER:
        // create JSON_NUMBER

    case TOKEN_STRING:
        // create JSON_STRING

    default:
        // error
}
```

---

# Milestone 9 — Memory Management

Strings require dynamic memory.

Implement things like:

```c
json_value_free()
```

For example:

```text
JsonValue
   │
   ├── type = JSON_STRING
   │
   └── string → "hello"
```

When finished:

```c
json_value_free(&value);
```

should release anything allocated by the parser.

This becomes especially important in v2 when arrays and objects introduce much more dynamic memory.

---

# Milestone 10 — Error Handling

Make invalid JSON fail cleanly.

Examples:

```text
"hello
```

```text
12.3.4
```

```text
tru
```

```text
nullx
```

You could eventually have:

```c
typedef enum
{
    JSON_SUCCESS,
    JSON_ERROR_UNEXPECTED_TOKEN,
    JSON_ERROR_INVALID_NUMBER,
    JSON_ERROR_INVALID_STRING,
    JSON_ERROR_UNEXPECTED_END
} JsonError;
```

---

# Milestone 11 — Tests

Build tests around each component.

### Scanner tests

```text
whitespace
empty input
character traversal
```

### Tokenizer tests

```text
{}
[]
:
,
true
false
null
123
"hello"
```

### Parser tests

```json
null
```

```json
true
```

```json
123.45
```

```json
"hello"
```

And invalid JSON:

```text
tru
"hello
12.3.4
```

---

# 🚀 Version 1 Definition of Done

I'd define **tinyjson v1** as:

```text
                  tinyjson v1
                      │
             ┌────────┴────────┐
             │                 │
         Tokenizer           Parser
             │                 │
             └────────┬────────┘
                      ↓
                  JsonValue
                      │
          ┌───────────┼───────────┐
          ↓           ↓           ↓
        null        bool       number
                                  │
                               string
```

It should be able to:

```c
json_parse("null");
json_parse("true");
json_parse("false");
json_parse("123.45");
json_parse("\"hello\"");
```

and reject malformed primitive JSON.

---

# 🏗️ Version 2 — Compound JSON

This is where your **non-primitive types** come in.

### Milestone 12 — Arrays

Support:

```json
[]
```

```json
[1, 2, 3]
```

```json
[true, false, null]
```

Eventually:

```json
[1, "hello", true, null]
```

And later nested structures:

```json
[[1, 2], [3, 4]]
```

---

### Milestone 13 — Objects

Support:

```json
{}
```

```json
{"name": "John"}
```

```json
{
    "name": "John",
    "age": 25
}
```

---

### Milestone 14 — Nested Values

Support:

```json
{
    "name": "John",
    "scores": [10, 20, 30]
}
```

and:

```json
[
    {"name": "John"},
    {"name": "Jane"}
]
```

This is where the recursive nature of JSON becomes important.

---

### Milestone 15 — Complete Memory Management

Handle freeing:

```text
JsonValue
   │
   ├── string
   │
   ├── array
   │    ├── JsonValue
   │    ├── JsonValue
   │    └── JsonValue
   │
   └── object
        ├── key → JsonValue
        ├── key → JsonValue
        └── key → JsonValue
```

`json_value_free()` will need to recursively free everything.

---

# 🌟 Version 3 — Quality & Features

After v2 works, you can consider:

```text
v3
├── Better error messages
├── Line/column error locations
├── UTF-8 / Unicode handling
├── More escape sequences
├── Pretty printing
├── JSON serialization
├── Streaming parser
├── Custom allocators
└── Performance improvements
```

## The overall journey

```text
                    tinyjson
                       │
            ┌──────────┴──────────┐
            │                     │
           V1                    V2
            │                     │
     Primitive JSON        Compound JSON
            │                     │
     ┌──────┼──────┐        ┌─────┴─────┐
     ↓      ↓      ↓        ↓           ↓
    null   bool   number   arrays     objects
                   │          │           │
                   ↓          └─────┬─────┘
                 string              ↓
                              nested JSON
```