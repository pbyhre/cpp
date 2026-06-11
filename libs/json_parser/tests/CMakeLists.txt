# Unit tests for json_parser library

include(TestFramework)

# Host-based test using Google Test
add_robotics_gtest(
    NAME test_json_parser_gtest
    SOURCES test_json_parser_gtest.cpp
    DEPENDENCIES robotics::json_parser
)

# Target test using simple assertions
add_robotics_target_test(
    NAME test_json_parser_target
    SOURCES test_json_parser_target.c
    DEPENDENCIES robotics::json_parser
)
