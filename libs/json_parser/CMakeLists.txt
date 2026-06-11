cmake_minimum_required(VERSION 3.20)
project(json_parser VERSION 0.1.0)

# Include common functions
include(CommonTargets)

# Library option (individual control)
option(BUILD_JSON_PARSER_LIB "Build json_parser library" ${BUILD_ALL_LIBS})

if(NOT BUILD_JSON_PARSER_LIB)
    return()
endif()

# Library sources
set(PUBLIC_HEADERS
    include/json_parser/json_parser.h
)

set(SOURCES
    src/json_parser.c
)

# Create library
add_robotics_library(
    NAME json_parser
    NAMESPACE robotics
    SOURCES ${SOURCES}
    PUBLIC_HEADERS ${PUBLIC_HEADERS}
)

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
