#include "json_parser/json_parser.h"
#include "test_assert.h"
#include <string.h>

static int event_count;

static bool count_callback(const json_event_data_t *event, void *user_data) {
    (void)event;
    (void)user_data;
    event_count++;
    return true;
}

static void test_parse_null(void) {
    TEST_BEGIN("parse_null");

    const char *json = "null";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 1);

    TEST_END();
}

static void test_parse_boolean(void) {
    TEST_BEGIN("parse_boolean");

    const char *json = "true";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 1);

    TEST_END();
}

static void test_parse_number(void) {
    TEST_BEGIN("parse_number");

    const char *json = "42";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 1);

    TEST_END();
}

static void test_parse_string(void) {
    TEST_BEGIN("parse_string");

    const char *json = "\"hello\"";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 1);

    TEST_END();
}

static void test_parse_empty_array(void) {
    TEST_BEGIN("parse_empty_array");

    const char *json = "[]";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 2);

    TEST_END();
}

static void test_parse_array_with_values(void) {
    TEST_BEGIN("parse_array_with_values");

    const char *json = "[1, 2, 3]";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 5);

    TEST_END();
}

static void test_parse_empty_object(void) {
    TEST_BEGIN("parse_empty_object");

    const char *json = "{}";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 2);

    TEST_END();
}

static void test_parse_object_with_key_value(void) {
    TEST_BEGIN("parse_object_with_key_value");

    const char *json = "{\"name\": \"test\"}";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 4);

    TEST_END();
}

static void test_invalid_syntax(void) {
    TEST_BEGIN("invalid_syntax");

    const char *json = "{invalid}";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_NE(err, JSON_OK);

    TEST_END();
}

static void test_unexpected_eof(void) {
    TEST_BEGIN("unexpected_eof");

    const char *json = "{\"key\":";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_ERROR_UNEXPECTED_EOF);

    TEST_END();
}

static void test_error_strings(void) {
    TEST_BEGIN("error_strings");

    const char *msg = json_error_string(JSON_OK);
    TEST_ASSERT_NOT_NULL(msg);

    msg = json_error_string(JSON_ERROR_INVALID_SYNTAX);
    TEST_ASSERT_NOT_NULL(msg);

    TEST_END();
}

static void test_whitespace_handling(void) {
    TEST_BEGIN("whitespace_handling");

    const char *json = "  \n\t  {  }  \n  ";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    event_count = 0;
    json_error_t err = json_parse(&parser, count_callback, NULL);

    TEST_ASSERT_EQ(err, JSON_OK);
    TEST_ASSERT_EQ(event_count, 2);

    TEST_END();
}

int main(void) {
    TEST_INIT();

    test_parse_null();
    test_parse_boolean();
    test_parse_number();
    test_parse_string();
    test_parse_empty_array();
    test_parse_array_with_values();
    test_parse_empty_object();
    test_parse_object_with_key_value();
    test_invalid_syntax();
    test_unexpected_eof();
    test_error_strings();
    test_whitespace_handling();

    return TEST_REPORT();
}
