#include "json_parser/json_parser.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

/* Forward declarations */
static json_error_t parse_value(json_parser_t *parser, json_callback_t callback, void *user_data);
static json_error_t parse_string(json_parser_t *parser, const char **str, size_t *len);
static json_error_t parse_number(json_parser_t *parser, double *value);
static json_error_t parse_object(json_parser_t *parser, json_callback_t callback, void *user_data);
static json_error_t parse_array(json_parser_t *parser, json_callback_t callback, void *user_data);

/* Helper macros */
#define PEEK(parser) (*(parser)->input)
#define NEXT(parser) do { \
    if ((parser)->remaining > 0) { \
        if (*(parser)->input == '\n') { \
            (parser)->line++; \
            (parser)->column = 0; \
        } else { \
            (parser)->column++; \
        } \
        (parser)->input++; \
        (parser)->remaining--; \
    } \
} while(0)

#define EXPECT(parser, ch) do { \
    if (PEEK(parser) != (ch)) { \
        (parser)->error = JSON_ERROR_INVALID_SYNTAX; \
        return (parser)->error; \
    } \
    NEXT(parser); \
} while(0)

void json_parser_init(json_parser_t *parser, const char *input, size_t length) {
    parser->input = input;
    parser->remaining = length;
    parser->depth = 0;
    parser->error = JSON_OK;
    parser->line = 1;
    parser->column = 0;
}

void json_skip_whitespace(json_parser_t *parser) {
    while (parser->remaining > 0 && isspace((unsigned char)PEEK(parser))) {
        NEXT(parser);
    }
}

const char *json_error_string(json_error_t error) {
    switch (error) {
        case JSON_OK: return "No error";
        case JSON_ERROR_INVALID_SYNTAX: return "Invalid syntax";
        case JSON_ERROR_UNEXPECTED_EOF: return "Unexpected end of input";
        case JSON_ERROR_TOO_DEEP: return "Nesting too deep";
        case JSON_ERROR_INVALID_NUMBER: return "Invalid number";
        case JSON_ERROR_INVALID_STRING: return "Invalid string";
        case JSON_ERROR_INVALID_VALUE: return "Invalid value";
        default: return "Unknown error";
    }
}

void json_get_position(const json_parser_t *parser, uint32_t *line, uint32_t *column) {
    if (line) *line = parser->line;
    if (column) *column = parser->column;
}

json_error_t json_parse(json_parser_t *parser, json_callback_t callback, void *user_data) {
    json_skip_whitespace(parser);
    json_error_t err = parse_value(parser, callback, user_data);
    if (err != JSON_OK) {
        return err;
    }

    json_skip_whitespace(parser);
    if (parser->remaining > 0) {
        parser->error = JSON_ERROR_INVALID_SYNTAX;
        return parser->error;
    }

    return JSON_OK;
}

static json_error_t parse_value(json_parser_t *parser, json_callback_t callback, void *user_data) {
    json_skip_whitespace(parser);

    if (parser->remaining == 0) {
        parser->error = JSON_ERROR_UNEXPECTED_EOF;
        return parser->error;
    }

    char ch = PEEK(parser);

    if (ch == '{') {
        return parse_object(parser, callback, user_data);
    } else if (ch == '[') {
        return parse_array(parser, callback, user_data);
    } else if (ch == '"') {
        const char *str;
        size_t len;
        json_error_t err = parse_string(parser, &str, &len);
        if (err != JSON_OK) {
            return err;
        }

        json_event_data_t event = {
            .event = JSON_EVENT_VALUE_STRING,
            .data.string = { .str = str, .len = len }
        };
        if (!callback(&event, user_data)) {
            return JSON_OK;
        }
        return JSON_OK;
    } else if (ch == 't' || ch == 'f') {
        /* Boolean */
        bool value;
        if (ch == 't') {
            if (parser->remaining < 4 || strncmp(parser->input, "true", 4) != 0) {
                parser->error = JSON_ERROR_INVALID_VALUE;
                return parser->error;
            }
            value = true;
            for (int i = 0; i < 4; i++) NEXT(parser);
        } else {
            if (parser->remaining < 5 || strncmp(parser->input, "false", 5) != 0) {
                parser->error = JSON_ERROR_INVALID_VALUE;
                return parser->error;
            }
            value = false;
            for (int i = 0; i < 5; i++) NEXT(parser);
        }

        json_event_data_t event = {
            .event = JSON_EVENT_VALUE_BOOL,
            .data.boolean = { .value = value }
        };
        if (!callback(&event, user_data)) {
            return JSON_OK;
        }
        return JSON_OK;
    } else if (ch == 'n') {
        /* null */
        if (parser->remaining < 4 || strncmp(parser->input, "null", 4) != 0) {
            parser->error = JSON_ERROR_INVALID_VALUE;
            return parser->error;
        }
        for (int i = 0; i < 4; i++) NEXT(parser);

        json_event_data_t event = { .event = JSON_EVENT_VALUE_NULL };
        if (!callback(&event, user_data)) {
            return JSON_OK;
        }
        return JSON_OK;
    } else if (ch == '-' || isdigit((unsigned char)ch)) {
        /* Number */
        double value;
        json_error_t err = parse_number(parser, &value);
        if (err != JSON_OK) {
            return err;
        }

        json_event_data_t event = {
            .event = JSON_EVENT_VALUE_NUMBER,
            .data.number = { .value = value }
        };
        if (!callback(&event, user_data)) {
            return JSON_OK;
        }
        return JSON_OK;
    } else {
        parser->error = JSON_ERROR_INVALID_VALUE;
        return parser->error;
    }
}

static json_error_t parse_string(json_parser_t *parser, const char **str, size_t *len) {
    EXPECT(parser, '"');

    const char *start = parser->input;
    size_t length = 0;

    while (parser->remaining > 0 && PEEK(parser) != '"') {
        if (PEEK(parser) == '\\') {
            NEXT(parser);
            if (parser->remaining == 0) {
                parser->error = JSON_ERROR_INVALID_STRING;
                return parser->error;
            }
        }
        NEXT(parser);
        length++;
    }

    if (parser->remaining == 0) {
        parser->error = JSON_ERROR_UNEXPECTED_EOF;
        return parser->error;
    }

    EXPECT(parser, '"');

    *str = start;
    *len = length;
    return JSON_OK;
}

static json_error_t parse_number(json_parser_t *parser, double *value) {
    const char *start = parser->input;
    size_t length = 0;

    /* Optional minus */
    if (PEEK(parser) == '-') {
        NEXT(parser);
        length++;
    }

    /* Integer part */
    if (parser->remaining == 0 || !isdigit((unsigned char)PEEK(parser))) {
        parser->error = JSON_ERROR_INVALID_NUMBER;
        return parser->error;
    }

    while (parser->remaining > 0 && isdigit((unsigned char)PEEK(parser))) {
        NEXT(parser);
        length++;
    }

    /* Fractional part */
    if (parser->remaining > 0 && PEEK(parser) == '.') {
        NEXT(parser);
        length++;

        if (parser->remaining == 0 || !isdigit((unsigned char)PEEK(parser))) {
            parser->error = JSON_ERROR_INVALID_NUMBER;
            return parser->error;
        }

        while (parser->remaining > 0 && isdigit((unsigned char)PEEK(parser))) {
            NEXT(parser);
            length++;
        }
    }

    /* Exponent part */
    if (parser->remaining > 0 && (PEEK(parser) == 'e' || PEEK(parser) == 'E')) {
        NEXT(parser);
        length++;

        if (parser->remaining > 0 && (PEEK(parser) == '+' || PEEK(parser) == '-')) {
            NEXT(parser);
            length++;
        }

        if (parser->remaining == 0 || !isdigit((unsigned char)PEEK(parser))) {
            parser->error = JSON_ERROR_INVALID_NUMBER;
            return parser->error;
        }

        while (parser->remaining > 0 && isdigit((unsigned char)PEEK(parser))) {
            NEXT(parser);
            length++;
        }
    }

    /* Convert to double */
    char buffer[64];
    if (length >= sizeof(buffer)) {
        parser->error = JSON_ERROR_INVALID_NUMBER;
        return parser->error;
    }
    memcpy(buffer, start, length);
    buffer[length] = '\0';

    *value = strtod(buffer, NULL);
    return JSON_OK;
}

static json_error_t parse_object(json_parser_t *parser, json_callback_t callback, void *user_data) {
    EXPECT(parser, '{');

    if (++parser->depth > JSON_MAX_NESTING) {
        parser->error = JSON_ERROR_TOO_DEEP;
        return parser->error;
    }

    json_event_data_t event = { .event = JSON_EVENT_OBJECT_START };
    if (!callback(&event, user_data)) {
        parser->depth--;
        return JSON_OK;
    }

    json_skip_whitespace(parser);

    if (parser->remaining > 0 && PEEK(parser) == '}') {
        NEXT(parser);
        parser->depth--;
        event.event = JSON_EVENT_OBJECT_END;
        callback(&event, user_data);
        return JSON_OK;
    }

    while (true) {
        json_skip_whitespace(parser);

        /* Parse key */
        const char *key;
        size_t key_len;
        json_error_t err = parse_string(parser, &key, &key_len);
        if (err != JSON_OK) {
            parser->depth--;
            return err;
        }

        event.event = JSON_EVENT_KEY;
        event.data.string.str = key;
        event.data.string.len = key_len;
        if (!callback(&event, user_data)) {
            parser->depth--;
            return JSON_OK;
        }

        json_skip_whitespace(parser);
        EXPECT(parser, ':');

        /* Parse value */
        err = parse_value(parser, callback, user_data);
        if (err != JSON_OK) {
            parser->depth--;
            return err;
        }

        json_skip_whitespace(parser);

        if (parser->remaining == 0) {
            parser->error = JSON_ERROR_UNEXPECTED_EOF;
            parser->depth--;
            return parser->error;
        }

        if (PEEK(parser) == '}') {
            NEXT(parser);
            break;
        }

        EXPECT(parser, ',');
    }

    parser->depth--;
    event.event = JSON_EVENT_OBJECT_END;
    callback(&event, user_data);
    return JSON_OK;
}

static json_error_t parse_array(json_parser_t *parser, json_callback_t callback, void *user_data) {
    EXPECT(parser, '[');

    if (++parser->depth > JSON_MAX_NESTING) {
        parser->error = JSON_ERROR_TOO_DEEP;
        return parser->error;
    }

    json_event_data_t event = { .event = JSON_EVENT_ARRAY_START };
    if (!callback(&event, user_data)) {
        parser->depth--;
        return JSON_OK;
    }

    json_skip_whitespace(parser);

    if (parser->remaining > 0 && PEEK(parser) == ']') {
        NEXT(parser);
        parser->depth--;
        event.event = JSON_EVENT_ARRAY_END;
        callback(&event, user_data);
        return JSON_OK;
    }

    while (true) {
        json_error_t err = parse_value(parser, callback, user_data);
        if (err != JSON_OK) {
            parser->depth--;
            return err;
        }

        json_skip_whitespace(parser);

        if (parser->remaining == 0) {
            parser->error = JSON_ERROR_UNEXPECTED_EOF;
            parser->depth--;
            return parser->error;
        }

        if (PEEK(parser) == ']') {
            NEXT(parser);
            break;
        }

        EXPECT(parser, ',');
        json_skip_whitespace(parser);
    }

    parser->depth--;
    event.event = JSON_EVENT_ARRAY_END;
    callback(&event, user_data);
    return JSON_OK;
}
