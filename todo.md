Added to the roadmap. The step is:

### V1 — Parser / Value Layer

**Convert numeric token text into C numeric values**

For example:

```text
JSON text
   ↓
Tokenizer
   ↓
TOKEN_NUMBER: "28.5"
   ↓
Parser / Value layer
   ↓
double: 28.5
```

We'll eventually use something like `strtod()` for the conversion.

Your updated relevant roadmap is:

1. Project foundation
2. Character scanner
3. Tokenizer
4. String parsing
5. Number tokenization
6. **Number conversion: `TOKEN_NUMBER` → `double`**
7. Literals: `true`, `false`, `null`
8. `JsonValue`
9. Parser
10. Memory management
11. Error handling
12. Tests

And importantly, **number tokenization and number conversion are separate steps**:

```text
"28.5"
   │
   ├── tokenizer → identifies it as a number
   │
   └── parser/value layer → converts it to double
```

That distinction will be useful when we build the parser.
