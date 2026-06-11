# json_parser

Lightweight streaming JSON parser for embedded and robotics systems.

## Features

- **Event-driven API**: Callback-based parsing, no DOM tree
- **Zero-copy**: String values point into the original buffer
- **No dynamic allocation**: All memory provided by caller
- **Incremental parsing**: Suitable for streaming data
- **Configurable nesting depth**: `JSON_MAX_NESTING` (default: 32)
- **Error reporting**: Line and column numbers for debugging

## Architecture

The parser uses a streaming, event-based approach similar to SAX parsers for XML. Instead of building a DOM tree in memory, it emits events as it encounters JSON elements.

**Benefits for embedded systems:**
- Minimal memory footprint
- Predictable memory usage
- Can parse large JSON without buffering entire document
- Suitable for parsing sensor data, config files, API responses

## Usage

### Basic Parsing

```c
#include "json_parser/json_parser.h"

bool my_callback(const json_event_data_t *event, void *user_data) {
    switch (event->event) {
        case JSON_EVENT_VALUE_NUMBER:
            printf("Number: %g\n", event->data.number.value);
            break;
        case JSON_EVENT_VALUE_STRING:
            printf("String: %.*s\n", 
                   (int)event->data.string.len, 
                   event->data.string.str);
            break;
        // Handle other events...
    }
    return true;  // Continue parsing
}

const char *json = "{\"temperature\": 22.5}";
json_parser_t parser;
json_parser_init(&parser, json, strlen(json));

json_error_t err = json_parse(&parser, my_callback, NULL);
if (err != JSON_OK) {
    printf("Error: %s\n", json_error_string(err));
}
```

### Extracting Specific Values

```c
typedef struct {
    const char *search_key;
    double value;
    bool found;
    bool next_is_target;
} extract_ctx_t;

bool extract_callback(const json_event_data_t *event, void *user_data) {
    extract_ctx_t *ctx = (extract_ctx_t *)user_data;
    
    if (event->event == JSON_EVENT_KEY) {
        if (strncmp(event->data.string.str, ctx->search_key, 
                    event->data.string.len) == 0) {
            ctx->next_is_target = true;
        }
    } else if (ctx->next_is_target && event->event == JSON_EVENT_VALUE_NUMBER) {
        ctx->value = event->data.number.value;
        ctx->found = true;
        return false;  // Stop parsing
    }
    
    return true;
}
```

### Event Types

| Event | Description | Data Available |
|-------|-------------|----------------|
| `JSON_EVENT_OBJECT_START` | `{` encountered | - |
| `JSON_EVENT_OBJECT_END` | `}` encountered | - |
| `JSON_EVENT_ARRAY_START` | `[` encountered | - |
| `JSON_EVENT_ARRAY_END` | `]` encountered | - |
| `JSON_EVENT_KEY` | Object key | `string.str`, `string.len` |
| `JSON_EVENT_VALUE_NULL` | `null` value | - |
| `JSON_EVENT_VALUE_BOOL` | Boolean | `boolean.value` |
| `JSON_EVENT_VALUE_NUMBER` | Number | `number.value` |
| `JSON_EVENT_VALUE_STRING` | String | `string.str`, `string.len` |

## Limitations

- **No escape sequence unescaping**: Strings are returned as-is from the input buffer. If you need to handle `\n`, `\t`, `\"`, etc., you must unescape them yourself.
- **No Unicode validation**: UTF-8 sequences are not validated
- **Fixed nesting depth**: Configurable via `JSON_MAX_NESTING`, but not dynamic
- **No in-place modification**: Parser requires the input buffer to remain valid during parsing

## Error Handling

```c
json_error_t err = json_parse(&parser, callback, user_data);
if (err != JSON_OK) {
    printf("Parse error: %s\n", json_error_string(err));
    
    uint32_t line, column;
    json_get_position(&parser, &line, &column);
    printf("At line %u, column %u\n", line, column);
}
```

**Error Codes:**
- `JSON_OK`: Success
- `JSON_ERROR_INVALID_SYNTAX`: Malformed JSON
- `JSON_ERROR_UNEXPECTED_EOF`: Input ended unexpectedly
- `JSON_ERROR_TOO_DEEP`: Nesting exceeds `JSON_MAX_NESTING`
- `JSON_ERROR_INVALID_NUMBER`: Malformed number
- `JSON_ERROR_INVALID_STRING`: Malformed string
- `JSON_ERROR_INVALID_VALUE`: Invalid value (not null/true/false/number/string)

## Building

```bash
# Build as part of monorepo
cmake -S . -B build
cmake --build build

# Build standalone
cmake -S libs/json_parser -B build/json_parser
cmake --build build/json_parser

# Run tests
ctest --test-dir build/json_parser
```

## Performance Considerations

- **Zero-copy strings**: String data is not copied, just referenced
- **Streaming**: No need to load entire JSON into memory
- **Early termination**: Return `false` from callback to stop parsing
- **No recursion overhead**: Uses explicit stack for nesting tracking

## Thread Safety

The parser is **not thread-safe**. Each thread should use its own `json_parser_t` instance.
