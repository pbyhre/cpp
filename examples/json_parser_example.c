#include "json_parser/json_parser.h"
#include <stdio.h>
#include <string.h>

/* Example: Pretty-print JSON with indentation */

typedef struct {
    int indent_level;
} print_context_t;

static void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
}

static bool pretty_print_callback(const json_event_data_t *event, void *user_data) {
    print_context_t *ctx = (print_context_t *)user_data;

    switch (event->event) {
        case JSON_EVENT_OBJECT_START:
            printf("{\n");
            ctx->indent_level++;
            break;

        case JSON_EVENT_OBJECT_END:
            ctx->indent_level--;
            print_indent(ctx->indent_level);
            printf("}");
            if (ctx->indent_level > 0) {
                printf("\n");
            }
            break;

        case JSON_EVENT_ARRAY_START:
            printf("[\n");
            ctx->indent_level++;
            break;

        case JSON_EVENT_ARRAY_END:
            ctx->indent_level--;
            print_indent(ctx->indent_level);
            printf("]");
            if (ctx->indent_level > 0) {
                printf("\n");
            }
            break;

        case JSON_EVENT_KEY:
            print_indent(ctx->indent_level);
            printf("\"%.*s\": ", (int)event->data.string.len, event->data.string.str);
            break;

        case JSON_EVENT_VALUE_NULL:
            if (ctx->indent_level > 0) {
                print_indent(ctx->indent_level);
            }
            printf("null\n");
            break;

        case JSON_EVENT_VALUE_BOOL:
            if (ctx->indent_level > 0) {
                print_indent(ctx->indent_level);
            }
            printf("%s\n", event->data.boolean.value ? "true" : "false");
            break;

        case JSON_EVENT_VALUE_NUMBER:
            if (ctx->indent_level > 0) {
                print_indent(ctx->indent_level);
            }
            printf("%g\n", event->data.number.value);
            break;

        case JSON_EVENT_VALUE_STRING:
            if (ctx->indent_level > 0) {
                print_indent(ctx->indent_level);
            }
            printf("\"%.*s\"\n", (int)event->data.string.len, event->data.string.str);
            break;

        case JSON_EVENT_ERROR:
            break;
    }

    return true;
}

/* Example: Extract specific value from JSON */

typedef struct {
    const char *search_key;
    bool found;
    double value;
    bool next_is_target;
} extract_context_t;

static bool extract_callback(const json_event_data_t *event, void *user_data) {
    extract_context_t *ctx = (extract_context_t *)user_data;

    if (event->event == JSON_EVENT_KEY) {
        if (event->data.string.len == strlen(ctx->search_key) &&
            strncmp(event->data.string.str, ctx->search_key, event->data.string.len) == 0) {
            ctx->next_is_target = true;
        }
    } else if (ctx->next_is_target && event->event == JSON_EVENT_VALUE_NUMBER) {
        ctx->value = event->data.number.value;
        ctx->found = true;
        ctx->next_is_target = false;
        return false;  /* Stop parsing */
    }

    return true;
}

/* Example: Sum array elements */

typedef struct {
    double sum;
} sum_context_t;

static bool sum_callback(const json_event_data_t *event, void *user_data) {
    sum_context_t *ctx = (sum_context_t *)user_data;

    if (event->event == JSON_EVENT_VALUE_NUMBER) {
        ctx->sum += event->data.number.value;
    }

    return true;
}

int main(void) {
    printf("JSON Parser Examples\n");
    printf("====================\n\n");

    /* Example 1: Pretty-print JSON */
    printf("Example 1: Pretty-print JSON\n");
    printf("----------------------------\n");

    const char *json1 = "{\"name\":\"Robot\",\"sensors\":[\"lidar\",\"camera\"],\"active\":true,\"speed\":1.5}";

    printf("Input: %s\n\n", json1);
    printf("Pretty-printed:\n");

    json_parser_t parser;
    json_parser_init(&parser, json1, strlen(json1));

    print_context_t print_ctx = { .indent_level = 0 };
    json_error_t err = json_parse(&parser, pretty_print_callback, &print_ctx);

    if (err != JSON_OK) {
        printf("\nError: %s\n", json_error_string(err));
        uint32_t line, column;
        json_get_position(&parser, &line, &column);
        printf("At line %u, column %u\n", line, column);
    } else {
        printf("\n");
    }

    /* Example 2: Extract specific value */
    printf("\nExample 2: Extract specific value\n");
    printf("----------------------------------\n");

    const char *json2 = "{\"temperature\":22.5,\"humidity\":65,\"pressure\":1013.25}";

    printf("Input: %s\n", json2);
    printf("Searching for key: \"temperature\"\n\n");

    json_parser_init(&parser, json2, strlen(json2));

    extract_context_t extract_ctx = {
        .search_key = "temperature",
        .found = false,
        .value = 0.0,
        .next_is_target = false
    };

    err = json_parse(&parser, extract_callback, &extract_ctx);

    if (extract_ctx.found) {
        printf("Found: temperature = %g\n", extract_ctx.value);
    } else {
        printf("Key not found\n");
    }

    /* Example 3: Parse array of numbers */
    printf("\nExample 3: Parse array of numbers\n");
    printf("----------------------------------\n");

    const char *json3 = "[10, 20, 30, 40, 50]";
    printf("Input: %s\n\n", json3);

    json_parser_init(&parser, json3, strlen(json3));

    printf("Sum of array elements: ");

    sum_context_t sum_ctx = { .sum = 0.0 };
    err = json_parse(&parser, sum_callback, &sum_ctx);
    printf("%g\n", sum_ctx.sum);

    /* Example 4: Error handling */
    printf("\nExample 4: Error handling\n");
    printf("-------------------------\n");

    const char *invalid_json = "{\"key\": invalid_value}";
    printf("Input: %s\n", invalid_json);

    json_parser_init(&parser, invalid_json, strlen(invalid_json));

    print_ctx.indent_level = 0;
    err = json_parse(&parser, pretty_print_callback, &print_ctx);

    if (err != JSON_OK) {
        printf("\nError detected: %s\n", json_error_string(err));
        uint32_t line, column;
        json_get_position(&parser, &line, &column);
        printf("At line %u, column %u\n", line, column);
    }

    printf("\nExamples completed!\n");
    return 0;
}
