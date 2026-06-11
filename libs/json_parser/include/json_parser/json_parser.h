#ifndef ROBOTICS_JSON_PARSER_JSON_PARSER_H
#define ROBOTICS_JSON_PARSER_JSON_PARSER_H

/**
 * @file json_parser.h
 * @brief Lightweight streaming JSON parser for embedded systems
 *
 * Event-driven parser with callback-based API. No dynamic memory allocation.
 * Suitable for parsing JSON from streams, buffers, or incrementally.
 *
 * Features:
 * - Zero-copy string parsing (pointers into original buffer)
 * - Configurable maximum nesting depth
 * - Incremental parsing support
 * - No heap allocation
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Parser configuration */
#ifndef JSON_MAX_NESTING
#define JSON_MAX_NESTING 32  /**< Maximum nesting depth for objects/arrays */
#endif

/* JSON value types */
typedef enum {
    JSON_TYPE_NULL,
    JSON_TYPE_BOOL,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT
} json_type_t;

/* Parser events */
typedef enum {
    JSON_EVENT_OBJECT_START,    /**< Object '{' encountered */
    JSON_EVENT_OBJECT_END,      /**< Object '}' encountered */
    JSON_EVENT_ARRAY_START,     /**< Array '[' encountered */
    JSON_EVENT_ARRAY_END,       /**< Array ']' encountered */
    JSON_EVENT_KEY,             /**< Object key (string) */
    JSON_EVENT_VALUE_NULL,      /**< null value */
    JSON_EVENT_VALUE_BOOL,      /**< Boolean value */
    JSON_EVENT_VALUE_NUMBER,    /**< Numeric value */
    JSON_EVENT_VALUE_STRING,    /**< String value */
    JSON_EVENT_ERROR            /**< Parse error */
} json_event_t;

/* Error codes */
typedef enum {
    JSON_OK = 0,
    JSON_ERROR_INVALID_SYNTAX,
    JSON_ERROR_UNEXPECTED_EOF,
    JSON_ERROR_TOO_DEEP,
    JSON_ERROR_INVALID_NUMBER,
    JSON_ERROR_INVALID_STRING,
    JSON_ERROR_INVALID_VALUE
} json_error_t;

/* Parser state */
typedef struct {
    const char *input;          /**< Current position in input */
    size_t remaining;           /**< Bytes remaining in input */
    int depth;                  /**< Current nesting depth */
    json_error_t error;         /**< Last error code */
    uint32_t line;              /**< Current line number (for error reporting) */
    uint32_t column;            /**< Current column number */
} json_parser_t;

/* Event data */
typedef struct {
    json_event_t event;
    union {
        struct {
            const char *str;    /**< Pointer to string in input buffer */
            size_t len;         /**< Length of string (not null-terminated) */
        } string;
        struct {
            double value;       /**< Numeric value */
        } number;
        struct {
            bool value;         /**< Boolean value */
        } boolean;
    } data;
} json_event_data_t;

/**
 * @brief Callback function for parser events
 *
 * @param event Event data
 * @param user_data User-provided context pointer
 * @return true to continue parsing, false to stop
 */
typedef bool (*json_callback_t)(const json_event_data_t *event, void *user_data);

/**
 * @brief Initialize JSON parser
 *
 * @param parser Parser state structure
 * @param input JSON input string
 * @param length Length of input
 */
void json_parser_init(json_parser_t *parser, const char *input, size_t length);

/**
 * @brief Parse JSON and invoke callbacks for each event
 *
 * @param parser Parser state
 * @param callback Function to call for each event
 * @param user_data User context passed to callback
 * @return JSON_OK on success, error code otherwise
 */
json_error_t json_parse(json_parser_t *parser, json_callback_t callback, void *user_data);

/**
 * @brief Get human-readable error message
 *
 * @param error Error code
 * @return Error message string
 */
const char *json_error_string(json_error_t error);

/**
 * @brief Skip whitespace in parser input
 *
 * @param parser Parser state
 */
void json_skip_whitespace(json_parser_t *parser);

/**
 * @brief Get current parser position (for error reporting)
 *
 * @param parser Parser state
 * @param line Output: line number
 * @param column Output: column number
 */
void json_get_position(const json_parser_t *parser, uint32_t *line, uint32_t *column);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTICS_JSON_PARSER_JSON_PARSER_H */
