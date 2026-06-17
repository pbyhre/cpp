# TestFramework.cmake
# Helpers for creating both host (gtest) and target (simple assertion) tests

# Create a host-based test using Google Test
function(add_robotics_gtest)
    set(options "")
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT BUILD_TESTS)
        return()
    endif()

    add_executable(${ARG_NAME} ${ARG_SOURCES})
    target_link_libraries(${ARG_NAME} PRIVATE
        GTest::gtest_main
        ${ARG_DEPENDENCIES}
    )
    apply_common_settings(${ARG_NAME})

    # Discover tests for CTest with increased timeout for Windows
    gtest_discover_tests(${ARG_NAME}
        DISCOVERY_TIMEOUT 30
    )
endfunction()

# Create a target test using simple assertions
function(add_robotics_target_test)
    set(options "")
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT BUILD_TESTS)
        return()
    endif()

    add_executable(${ARG_NAME} ${ARG_SOURCES})
    target_link_libraries(${ARG_NAME} PRIVATE ${ARG_DEPENDENCIES})
    target_include_directories(${ARG_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/cmake/test_framework
    )
    apply_common_settings(${ARG_NAME})

    # Add to CTest
    add_test(NAME ${ARG_NAME} COMMAND ${ARG_NAME})
endfunction()
