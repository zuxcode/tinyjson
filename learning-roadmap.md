# tinyjson — C Learning Roadmap

## Phase 1 — C Foundations Through the Tokenizer

### 1. Project & Build System

**Learn**

* `.c` vs `.h`
* compilation and linking
* CMake
* static libraries
* include paths
* compiler warnings

**Build**

```text
CMakeLists.txt
include/tinyjson/
src/
tests/
```

**Goal:** Build and run a minimal C project confidently.

---

### 2. Structs, Enums & `typedef`

**Learn**

* `struct`
* `enum`
* `typedef`
* `.field` access
* separating public types into headers

**Build**

```c
typedef enum
{
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct
{
    TokenType type;
    const char *start;
    size_t length;
} Token;
```

**Goal:** Understand how C represents structured data.

---

### 3. Pointers

**Learn**

* addresses
* `*`
* `&`
* pointer types
* pointer arithmetic
* `const char *`

**Build**

```c
const char *json = "{\"temperature\": 28.5}";
const char *current = json;
```

**Goal:** Be completely comfortable with:

```text
current
*current
&current
```

---

### 4. Double Pointers

**Learn**

* `char **`
* modifying a pointer through another pointer
* why `tokenizer_next(&current)` is necessary

**Build**

```c
Token tokenizer_next(const char **current);
```

Understand:

```text
current
   ↓
*current
   ↓
**current
```

**Goal:** Understand `const char **` rather than memorizing it.

---

# Phase 2 — Build the Tokenizer

### 5. Character Scanning

**Learn**

* `while`
* `if`
* character comparisons
* `'\0'`
* `EOF`
* `<ctype.h>`
* `isdigit()`

**Build**

```c
skip_whitespace()
```

and basic punctuation scanning.

**Goal:** Move through an input string safely.

---

### 6. Token Slices

**Learn**

* pointer subtraction
* `size_t`
* non-owning memory
* `%.*s`

**Build**

```c
typedef struct
{
    TokenType type;
    const char *start;
    size_t length;
} Token;
```

Understand:

```text
start + length
```

as a view into the original JSON.

**Goal:** Stop thinking that every token needs a copied string.

---

### 7. String Scanning

**Learn**

* scanning until a delimiter
* escaped characters
* backslashes
* validating input

**Build**

```c
scan_string()
```

Support:

```json
"hello"
"hello \"world\""
"hello \\ world"
"hello\nworld"
```

Eventually validate JSON escapes:

```text
\" \\ \/ \b \f \n \r \t \uXXXX
```

**Goal:** Understand stateful character scanning.

---

### 8. Number Scanning

**Learn**

* parsing a grammar manually
* optional components
* state/branching
* validating malformed input

**Build**

```c
scan_number()
```

Support:

```text
0
42
-42
3.14
-3.14
1e10
-2.5e-3
```

Reject:

```text
01
1.
1e
1e-
-
```

**Goal:** Learn how a tokenizer recognizes a structured grammar.

---

### 9. Literals

**Learn**

* fixed-length matching
* arrays of characters
* pointer indexing

**Build**

```c
scan_true()
scan_false()
scan_null()
```

For example:

```c
(*current)[0]
(*current)[1]
(*current)[2]
(*current)[3]
```

**Goal:** Understand how strings are inspected without copying them.

---

### 10. Tests

**Learn**

* assertions
* test organization
* expected failures
* regression testing

Build:

```text
tests/
├── test_tokenizer.c
├── test_numbers.c
├── test_strings.c
└── test_literals.c
```

Start with:

```c
assert(token.type == TOKEN_NUMBER);
```

Then deliberately create failing tests.

**Goal:** Develop the habit:

> **Write failing test → implement → make test pass → refactor.**

---

# Phase 3 — Understand the Parser

### 11. `JsonValue`

**Learn**

* unions
* tagged unions
* ownership
* dynamic memory

Build:

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

and:

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

Understand the difference between:

```text
TOKEN_NUMBER
```

and:

```text
JSON_NUMBER
```

---

### 12. Primitive Parser

**Learn**

* parser control flow
* converting tokens into values
* `strtod()`
* error propagation

Build:

```c
parse_value()
```

Handle:

```text
TOKEN_NULL
TOKEN_TRUE
TOKEN_FALSE
TOKEN_NUMBER
TOKEN_STRING
```

Example:

```text
"28.5"
   ↓
TOKEN_NUMBER
   ↓
strtod()
   ↓
double 28.5
   ↓
JSON_NUMBER
```

**Goal:** Understand the difference between **lexing** and **parsing**.

---

# Phase 4 — Recursive JSON

### 13. Arrays

**Learn**

* dynamic arrays
* `malloc`
* `realloc`
* `free`
* recursion

Parse:

```json
[1, 2, 3]
```

Then:

```json
[1, "hello", true, null]
```

Then:

```json
[[1, 2], [3, 4]]
```

**Goal:** Understand recursive data structures.

---

### 14. Objects

**Learn**

* key/value relationships
* dynamic structures
* ownership
* nested parsing

Parse:

```json
{
    "name": "John",
    "age": 30
}
```

Then:

```json
{
    "user": {
        "name": "John"
    }
}
```

**Goal:** Understand how a recursive parser represents real JSON.

---

### 15. Memory Management

**Learn**

* ownership
* allocation
* deallocation
* `malloc`
* `calloc`
* `realloc`
* `free`
* avoiding leaks
* use-after-free

Build:

```c
void json_free(JsonValue *value);
```

It should recursively free:

```text
object
 ├── key
 ├── value
 └── nested value
       └── ...
```

**Goal:** Be able to explain who owns every allocation.

---

# Phase 5 — Production Thinking

Once the parser works, **don't immediately optimize it**.

First make it reliable.

### 16. Error Handling

Learn:

* error types
* error positions
* propagating errors
* useful diagnostics

Move from:

```text
TOKEN_ERROR
```

to something like:

```text
invalid exponent at position 17
```

---

### 17. Fuzz Testing

Learn:

* fuzzing
* malformed input
* crash detection
* invariants

The fundamental requirement becomes:

```text
ANY INPUT
   ↓
tinyjson
   ↓
valid result OR clean error
```

**Never crash.**

---

### 18. Sanitizers

Learn:

```text
AddressSanitizer
UndefinedBehaviorSanitizer
```

Use them to find:

* buffer overflows
* use-after-free
* invalid memory access
* undefined behavior

---

### 19. Unicode & UTF-8

Learn:

* UTF-8
* Unicode code points
* JSON `\uXXXX`
* surrogate pairs
* byte vs character concepts

This is a particularly valuable C lesson because it forces you to understand the difference between:

```text
bytes
characters
code points
```

---

### 20. Resource Limits

Learn defensive programming.

Support limits such as:

```c
max_depth
max_string_length
max_input_length
```

Understand why this matters when parsing **untrusted input**.

---

# Phase 6 — Prepare to Ship

### 21. API Design

Turn internal implementation into a clean public API:

```c
JsonValue *json_parse(const char *input);
void json_free(JsonValue *value);
```

Keep implementation details private.

---

### 22. Documentation

Document:

* installation
* API
* supported JSON
* errors
* memory ownership
* limitations
* examples

---

### 23. Compatibility Testing

Compare `tinyjson` against mature JSON implementations.

Not as a dependency.

Instead:

```text
             same input
                 ↓
       ┌─────────┴─────────┐
       ↓                   ↓
   tinyjson          reference parser
       ↓                   ↓
       └─────────┬─────────┘
                 ↓
              compare
```

This is where a production library becomes much more trustworthy.

---

### 24. Performance

Only now learn:

* profiling
* allocations
* cache behavior
* benchmarks
* avoiding unnecessary copies
* parser throughput

Then optimize based on measurements rather than guesses.

---

# The Complete Learning Path

```text
C FUNDAMENTALS
      │
      ├── structs
      ├── enums
      ├── typedef
      ├── pointers
      ├── double pointers
      ├── arrays
      ├── strings
      └── memory
             │
             ▼
       TOKENIZER
             │
             ├── whitespace
             ├── punctuation
             ├── strings
             ├── numbers
             └── literals
                    │
                    ▼
                  TESTS
                    │
                    ▼
                JSON VALUE
                    │
                    ├── union
                    ├── ownership
                    └── dynamic memory
                           │
                           ▼
                    RECURSIVE PARSER
                           │
                           ├── primitives
                           ├── arrays
                           └── objects
                                  │
                                  ▼
                         MEMORY MANAGEMENT
                                  │
                                  ▼
                         ERROR HANDLING
                                  │
                                  ▼
                            FUZZ TESTING
                                  │
                                  ▼
                             SANITIZERS
                                  │
                                  ▼
                         UTF-8 / UNICODE
                                  │
                                  ▼
                          RESOURCE LIMITS
                                  │
                                  ▼
                            PUBLIC API
                                  │
                                  ▼
                         COMPATIBILITY TESTS
                                  │
                                  ▼
                            BENCHMARKS
                                  │
                                  ▼
                         PRODUCTION RELEASE
```

### Your current position

You're around **Phase 2, steps 7–9**:

```text
✅ project structure
✅ CMake
✅ structs/enums/typedef
✅ pointers
✅ double pointers
✅ whitespace
✅ punctuation
✅ basic strings
✅ numbers
✅ true
✅ false
🔨 null
🔨 strict number validation
🔨 robust string escapes
⬜ tokenizer tests
⬜ parser
⬜ JsonValue
```

```text
tinyjson
│
├── tokenizer
│   ├── strings
│   ├── numbers
│   ├── true
│   ├── false
│   └── null
│
├── parser
│   ├── primitive values
│   ├── arrays
│   └── objects
│
├── JsonValue
│
├── memory management
│
├── errors
│
└── tests
```

So **don't jump to arrays, objects, Unicode, or optimization yet**.

Your next learning milestone should be:

> **Finish a strict, well-tested tokenizer.**

Once that's solid, we'll move into `JsonValue` and the parser.
