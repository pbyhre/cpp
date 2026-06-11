cmake_minimum_required(VERSION 3.20)
project(data_structures VERSION 0.1.0)

# Include common functions
include(CommonTargets)

# Library option (individual control)
option(BUILD_DATA_STRUCTURES_LIB "Build data_structures library" ${BUILD_ALL_LIBS})

if(NOT BUILD_DATA_STRUCTURES_LIB)
    return()
endif()

# Library sources
set(PUBLIC_HEADERS
    include/data_structures/ring_buffer.h
)

set(SOURCES
    src/ring_buffer.c
)

# Create library
add_robotics_library(
    NAME data_structures
    NAMESPACE robotics
    SOURCES ${SOURCES}
    PUBLIC_HEADERS ${PUBLIC_HEADERS}
)

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
