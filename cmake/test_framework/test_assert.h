#ifndef ROBOTICS_TEST_ASSERT_H
#define ROBOTICS_TEST_ASSERT_H

/**
 * @file test_assert.h
 * @brief Lightweight assertion framework for embedded target testing
 *
 * Minimal footprint test assertions suitable for bare-metal and RTOS environments.
 * No dynamic allocation, no exceptions, no dependencies.
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Global test counters */
static int _test_passed = 0;
static int _test_failed = 0;
static const char *_current_test = NULL;

/* Initialize test framework */
#define TEST_INIT() do { \
    _test_passed = 0; \
    _test_failed = 0; \
    printf("Running tests...\n"); \
} while(0)

/* Begin a test */
#define TEST_BEGIN(name) do { \
    _current_test = name; \
    printf("  %s: ", name); \
    fflush(stdout); \
} while(0)

/* End a test */
#define TEST_END() do { \
    printf("PASSED\n"); \
    _test_passed++; \
    _current_test = NULL; \
} while(0)

/* Report test results and return appropriate exit code */
static inline int _test_report_results(void) {
    printf("\n");
    printf("Tests passed: %d\n", _test_passed);
    printf("Tests failed: %d\n", _test_failed);
    return (_test_failed == 0) ? 0 : 1;
}
#define TEST_REPORT() _test_report_results()

/* Assertion macros */
#define TEST_ASSERT(condition) do { \
    if (!(condition)) { \
        printf("FAILED\n"); \
        printf("    Assertion failed: %s\n", #condition); \
        printf("    File: %s:%d\n", __FILE__, __LINE__); \
        _test_failed++; \
        _current_test = NULL; \
        return; \
    } \
} while(0)

#define TEST_ASSERT_EQ(actual, expected) do { \
    if ((actual) != (expected)) { \
        printf("FAILED\n"); \
        printf("    Expected: %ld, got: %ld\n", (long)(expected), (long)(actual)); \
        printf("    File: %s:%d\n", __FILE__, __LINE__); \
        _test_failed++; \
        _current_test = NULL; \
        return; \
    } \
} while(0)

#define TEST_ASSERT_NE(actual, expected) do { \
    if ((actual) == (expected)) { \
        printf("FAILED\n"); \
        printf("    Expected not equal to: %ld\n", (long)(expected)); \
        printf("    File: %s:%d\n", __FILE__, __LINE__); \
        _test_failed++; \
        _current_test = NULL; \
        return; \
    } \
} while(0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))

#define TEST_ASSERT_NULL(ptr) do { \
    if ((ptr) != NULL) { \
        printf("FAILED\n"); \
        printf("    Expected NULL pointer\n"); \
        printf("    File: %s:%d\n", __FILE__, __LINE__); \
        _test_failed++; \
        _current_test = NULL; \
        return; \
    } \
} while(0)

#define TEST_ASSERT_NOT_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        printf("FAILED\n"); \
        printf("    Expected non-NULL pointer\n"); \
        printf("    File: %s:%d\n", __FILE__, __LINE__); \
        _test_failed++; \
        _current_test = NULL; \
        return; \
    } \
} while(0)

#define TEST_ASSERT_MEM_EQ(actual, expected, size) do { \
    if (memcmp((actual), (expected), (size)) != 0) { \
        printf("FAILED\n"); \
        printf("    Memory comparison failed\n"); \
        printf("    File: %s:%d\n", __FILE__, __LINE__); \
        _test_failed++; \
        _current_test = NULL; \
        return; \
    } \
} while(0)

#ifdef __cplusplus
}
#endif

#endif /* ROBOTICS_TEST_ASSERT_H */
