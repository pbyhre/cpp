#include "json_parser/json_parser.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>

struct TestEvent {
    json_event_t type;
    std::string string_value;
    double number_value;
    bool bool_value;

    TestEvent(json_event_t t) : type(t), number_value(0), bool_value(false) {}
    TestEvent(json_event_t t, const char *str, size_t len)
        : type(t), string_value(str, len), number_value(0), bool_value(false) {}
    TestEvent(json_event_t t, double num)
        : type(t), number_value(num), bool_value(false) {}
    TestEvent(json_event_t t, bool b)
        : type(t), number_value(0), bool_value(b) {}
};

class EventCollector {
public:
    std::vector<TestEvent> events;

    static bool callback(const json_event_data_t *event, void *user_data) {
        auto *collector = static_cast<EventCollector *>(user_data);

        switch (event->event) {
            case JSON_EVENT_OBJECT_START:
            case JSON_EVENT_OBJECT_END:
            case JSON_EVENT_ARRAY_START:
            case JSON_EVENT_ARRAY_END:
            case JSON_EVENT_VALUE_NULL:
                collector->events.emplace_back(event->event);
                break;
            case JSON_EVENT_KEY:
            case JSON_EVENT_VALUE_STRING:
                collector->events.emplace_back(
                    event->event,
                    event->data.string.str,
                    event->data.string.len
                );
                break;
            case JSON_EVENT_VALUE_NUMBER:
                collector->events.emplace_back(event->event, event->data.number.value);
                break;
            case JSON_EVENT_VALUE_BOOL:
                collector->events.emplace_back(event->event, event->data.boolean.value);
                break;
            case JSON_EVENT_ERROR:
                break;
        }
        return true;
    }
};

TEST(JsonParserTest, ParseNull) {
    const char *json = "null";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    json_error_t err = json_parse(&parser, EventCollector::callback, &collector);

    EXPECT_EQ(err, JSON_OK);
    ASSERT_EQ(collector.events.size(), 1);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_VALUE_NULL);
}

TEST(JsonParserTest, ParseBooleans) {
    const char *json_true = "true";
    const char *json_false = "false";

    json_parser_t parser;
    json_parser_init(&parser, json_true, strlen(json_true));
    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 1);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_VALUE_BOOL);
    EXPECT_TRUE(collector.events[0].bool_value);

    collector.events.clear();
    json_parser_init(&parser, json_false, strlen(json_false));
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 1);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_VALUE_BOOL);
    EXPECT_FALSE(collector.events[0].bool_value);
}

TEST(JsonParserTest, ParseNumbers) {
    const char *json = "42";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 1);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_VALUE_NUMBER);
    EXPECT_DOUBLE_EQ(collector.events[0].number_value, 42.0);
}

TEST(JsonParserTest, ParseNegativeNumber) {
    const char *json = "-123.456";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 1);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_VALUE_NUMBER);
    EXPECT_DOUBLE_EQ(collector.events[0].number_value, -123.456);
}

TEST(JsonParserTest, ParseScientificNotation) {
    const char *json = "1.23e10";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 1);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_VALUE_NUMBER);
    EXPECT_DOUBLE_EQ(collector.events[0].number_value, 1.23e10);
}

TEST(JsonParserTest, ParseString) {
    const char *json = "\"hello world\"";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 1);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_VALUE_STRING);
    EXPECT_EQ(collector.events[0].string_value, "hello world");
}

TEST(JsonParserTest, ParseEmptyArray) {
    const char *json = "[]";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 2);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_ARRAY_START);
    EXPECT_EQ(collector.events[1].type, JSON_EVENT_ARRAY_END);
}

TEST(JsonParserTest, ParseArrayWithValues) {
    const char *json = "[1, 2, 3]";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 5);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_ARRAY_START);
    EXPECT_EQ(collector.events[1].type, JSON_EVENT_VALUE_NUMBER);
    EXPECT_DOUBLE_EQ(collector.events[1].number_value, 1.0);
    EXPECT_EQ(collector.events[2].type, JSON_EVENT_VALUE_NUMBER);
    EXPECT_DOUBLE_EQ(collector.events[2].number_value, 2.0);
    EXPECT_EQ(collector.events[3].type, JSON_EVENT_VALUE_NUMBER);
    EXPECT_DOUBLE_EQ(collector.events[3].number_value, 3.0);
    EXPECT_EQ(collector.events[4].type, JSON_EVENT_ARRAY_END);
}

TEST(JsonParserTest, ParseEmptyObject) {
    const char *json = "{}";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 2);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_OBJECT_START);
    EXPECT_EQ(collector.events[1].type, JSON_EVENT_OBJECT_END);
}

TEST(JsonParserTest, ParseObjectWithKeyValue) {
    const char *json = "{\"name\": \"test\", \"value\": 42}";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 6);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_OBJECT_START);
    EXPECT_EQ(collector.events[1].type, JSON_EVENT_KEY);
    EXPECT_EQ(collector.events[1].string_value, "name");
    EXPECT_EQ(collector.events[2].type, JSON_EVENT_VALUE_STRING);
    EXPECT_EQ(collector.events[2].string_value, "test");
    EXPECT_EQ(collector.events[3].type, JSON_EVENT_KEY);
    EXPECT_EQ(collector.events[3].string_value, "value");
    EXPECT_EQ(collector.events[4].type, JSON_EVENT_VALUE_NUMBER);
    EXPECT_DOUBLE_EQ(collector.events[4].number_value, 42.0);
    EXPECT_EQ(collector.events[5].type, JSON_EVENT_OBJECT_END);
}

TEST(JsonParserTest, ParseNestedStructure) {
    const char *json = "{\"array\": [1, 2, {\"nested\": true}]}";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_GT(collector.events.size(), 0);
    EXPECT_EQ(collector.events[0].type, JSON_EVENT_OBJECT_START);
}

TEST(JsonParserTest, InvalidSyntax) {
    const char *json = "{invalid}";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    json_error_t err = json_parse(&parser, EventCollector::callback, &collector);
    EXPECT_NE(err, JSON_OK);
}

TEST(JsonParserTest, UnexpectedEOF) {
    const char *json = "{\"key\":";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    json_error_t err = json_parse(&parser, EventCollector::callback, &collector);
    EXPECT_EQ(err, JSON_ERROR_UNEXPECTED_EOF);
}

TEST(JsonParserTest, ErrorStrings) {
    EXPECT_STREQ(json_error_string(JSON_OK), "No error");
    EXPECT_STREQ(json_error_string(JSON_ERROR_INVALID_SYNTAX), "Invalid syntax");
    EXPECT_STREQ(json_error_string(JSON_ERROR_UNEXPECTED_EOF), "Unexpected end of input");
}

TEST(JsonParserTest, WhitespaceHandling) {
    const char *json = "  \n\t  {  \"key\"  :  \"value\"  }  \n  ";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 4);
}

TEST(JsonParserTest, MixedTypes) {
    const char *json = "[null, true, false, 123, \"text\"]";
    json_parser_t parser;
    json_parser_init(&parser, json, strlen(json));

    EventCollector collector;
    EXPECT_EQ(json_parse(&parser, EventCollector::callback, &collector), JSON_OK);
    ASSERT_EQ(collector.events.size(), 7);
    EXPECT_EQ(collector.events[1].type, JSON_EVENT_VALUE_NULL);
    EXPECT_EQ(collector.events[2].type, JSON_EVENT_VALUE_BOOL);
    EXPECT_EQ(collector.events[3].type, JSON_EVENT_VALUE_BOOL);
    EXPECT_EQ(collector.events[4].type, JSON_EVENT_VALUE_NUMBER);
    EXPECT_EQ(collector.events[5].type, JSON_EVENT_VALUE_STRING);
}
