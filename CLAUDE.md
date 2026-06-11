# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a monorepo containing a collection of cross-platform C/C++ libraries for embedded and robotics development. Libraries cover data manipulation, networking, math, hardware abstraction, and other foundational components. All libraries adhere to common style guidelines and build system conventions.

## Architecture

**Monorepo with Modular Components:**
- Each library in `libs/` is independently buildable but shares common infrastructure
- Libraries can be built individually or all together from the root
- Target-based dependency management using modern CMake (3.20+)
- All libraries use the `robotics::` namespace

**Directory Structure:**
```
cpp/
├── CMakeLists.txt              # Root build orchestrator
├── cmake/                      # Shared CMake infrastructure
│   ├── CompilerSettings.cmake  # Common compiler flags and options
│   ├── EmbeddedToolchains.cmake # Embedded target helpers
│   ├── CommonTargets.cmake     # Helper functions for creating libraries
│   └── toolchains/             # Cross-compilation toolchain files
├── libs/                       # Individual libraries
│   └── <library_name>/
│       ├── CMakeLists.txt      # Library-specific build
│       ├── include/            # Public headers
│       ├── src/                # Implementation
│       ├── tests/              # Unit tests
│       └── README.md           # Library documentation
├── examples/                   # Cross-library usage examples
└── tools/                      # Build scripts and utilities
```

## Building

**Build all libraries:**
```bash
cmake -S . -B build
cmake --build build
```

**Build specific library:**
```bash
cmake -S libs/data_structures -B build/data_structures
cmake --build build/data_structures
```

**Build options:**
- `BUILD_ALL_LIBS=ON`: Build all libraries (default: ON)
- `BUILD_TESTS=ON`: Build unit tests (default: ON)
- `BUILD_EXAMPLES=ON`: Build examples (default: ON)
- `BUILD_SHARED_LIBS=OFF`: Build shared libraries instead of static (default: OFF)
- `ROBOTICS_NO_EXCEPTIONS=OFF`: Disable C++ exceptions for embedded targets
- `ROBOTICS_NO_RTTI=OFF`: Disable RTTI for embedded targets
- `ROBOTICS_NO_HEAP=OFF`: Build without dynamic memory allocation

**Cross-compilation:**
```bash
cmake -S . -B build-arm -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake
cmake --build build-arm
```

**Run tests:**
```bash
ctest --test-dir build
```

**Run a single test:**
```bash
ctest --test-dir build -R ring_buffer
```

## Code Style and Conventions

**Formatting:**
- Use `clang-format` with the project's `.clang-format` configuration
- Run: `clang-format -i <files>`
- 4 spaces for indentation, 100 character line limit
- Linux-style braces for functions, same-line for control flow

**Linting:**
- Use `clang-tidy` with the project's `.clang-tidy` configuration
- Run: `clang-tidy <files> -- -I<include_dirs>`

**Naming Conventions:**
- Functions: `lower_case_with_underscores`
- Variables: `lower_case_with_underscores`
- Constants/Macros: `UPPER_CASE_WITH_UNDERSCORES`
- Types (structs/classes/typedefs): `lower_case_with_underscores_t` suffix
- Namespaces: `lower_case`
- Header guards: `ROBOTICS_<LIBRARY>_<FILE>_H` or `#pragma once`

**Library Naming:**
- Library namespace: `robotics::<library_name>`
- Include pattern: `#include "<library_name>/<header>.h"`
- CMake target: `robotics::<library_name>`

## Embedded/Robotics Design Principles

**Resource Constraints:**
- Avoid dynamic memory allocation where possible - provide user-supplied backing storage
- Make features opt-in (exceptions, RTTI, heap usage)
- Document memory footprint and stack usage for functions
- Use fixed-size buffers or allocators with known bounds

**Real-Time Considerations:**
- Document whether functions are lock-free, wait-free, or bounded time
- Avoid blocking operations in core data structures
- Provide interrupt-safe variants where applicable

**Cross-Platform:**
- Abstract platform-specific code behind interfaces
- Support bare-metal, RTOS, and hosted environments
- Use standard C/C++ where possible, provide fallbacks for freestanding environments

**Hardware Abstraction:**
- Platform-specific code isolated in `embedded_hal` library
- Provide clear interfaces for peripheral access (GPIO, SPI, I2C, etc.)

## Adding a New Library

1. Create directory structure: `libs/<library_name>/{include,src,tests}`
2. Create `libs/<library_name>/CMakeLists.txt` using `add_robotics_library()` helper
3. Add library to root `CMakeLists.txt`: `add_subdirectory(libs/<library_name>)`
4. Create public headers in `include/<library_name>/`
5. Add tests in `tests/` subdirectory
6. Document in `libs/<library_name>/README.md`

**Example library CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(my_library VERSION 0.1.0)

include(CommonTargets)

option(BUILD_MY_LIBRARY_LIB "Build my_library" ${BUILD_ALL_LIBS})

if(NOT BUILD_MY_LIBRARY_LIB)
    return()
endif()

set(PUBLIC_HEADERS
    include/my_library/header.h
)

set(SOURCES
    src/implementation.c
)

add_robotics_library(
    NAME my_library
    NAMESPACE robotics
    SOURCES ${SOURCES}
    PUBLIC_HEADERS ${PUBLIC_HEADERS}
    DEPENDENCIES robotics::other_library  # Optional
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
```

## Dependencies Between Libraries

- Use target-based dependencies: `target_link_libraries(mylib PUBLIC robotics::other_lib)`
- Keep dependency graph acyclic
- Document dependencies in library README.md
- Consider creating facade libraries for common combinations

## Testing

**Hybrid Testing Strategy:**
This project uses two complementary testing approaches:

1. **Host-based tests (Google Test)** - For development and CI
2. **Target tests (simple assertions)** - For on-target validation

### Host-Based Testing with Google Test

Use Google Test for comprehensive testing during development:

```cmake
# In libs/<library>/tests/CMakeLists.txt
include(TestFramework)

add_robotics_gtest(
    NAME test_mylib_gtest
    SOURCES test_mylib_gtest.cpp
    DEPENDENCIES robotics::mylib
)
```

**Features:**
- Rich assertions: `EXPECT_EQ`, `ASSERT_TRUE`, `EXPECT_NEAR`, etc.
- Test fixtures for setup/teardown
- Parameterized tests for testing multiple configurations
- Better failure reporting and test discovery
- Runs on development host (fast iteration)

**When to use:**
- During development and debugging
- CI/CD pipelines
- Complex test scenarios requiring mocking
- Performance benchmarking

### Target Testing with Simple Assertions

Use lightweight assertions for on-target validation:

```cmake
add_robotics_target_test(
    NAME test_mylib_target
    SOURCES test_mylib_target.c
    DEPENDENCIES robotics::mylib
)
```

**Implementation:**
```c
#include "test_assert.h"

static void test_feature(void) {
    TEST_BEGIN("feature");
    
    // Test code here
    TEST_ASSERT_EQ(actual, expected);
    TEST_ASSERT_TRUE(condition);
    TEST_ASSERT_MEM_EQ(buf1, buf2, size);
    
    TEST_END();
}

int main(void) {
    TEST_INIT();
    test_feature();
    return TEST_REPORT();
}
```

**Available assertions:**
- `TEST_ASSERT(condition)`
- `TEST_ASSERT_EQ(actual, expected)`
- `TEST_ASSERT_NE(actual, expected)`
- `TEST_ASSERT_TRUE(condition)`, `TEST_ASSERT_FALSE(condition)`
- `TEST_ASSERT_NULL(ptr)`, `TEST_ASSERT_NOT_NULL(ptr)`
- `TEST_ASSERT_MEM_EQ(actual, expected, size)`

**Features:**
- Minimal footprint (~200 bytes)
- No dependencies (just stdio)
- No exceptions, no heap allocation
- Works in bare-metal and RTOS environments

**When to use:**
- Final validation on target hardware
- Bare-metal environments
- Resource-constrained targets
- Hardware-specific behavior validation

### Running Tests

**All tests:**
```bash
ctest --test-dir build
```

**Only Google Test tests:**
```bash
ctest --test-dir build -R gtest
```

**Only target tests:**
```bash
ctest --test-dir build -R target
```

**Specific test:**
```bash
ctest --test-dir build -R ring_buffer_gtest
./build/libs/data_structures/tests/test_ring_buffer_gtest  # Direct execution
```

### Test Organization

Each library should have both test types in `libs/<library>/tests/`:
- `test_<feature>_gtest.cpp` - Google Test version
- `test_<feature>_target.c` - Target test version

Tests should cover:
- Edge cases: empty inputs, boundary conditions, wrap-around
- Error handling and invalid inputs
- Typical usage scenarios
- Platform-specific behavior (for target tests)

## Common Pitfalls

- Don't use global variables in library code
- Avoid standard library in bare-metal code unless documented
- Always check return values and handle errors
- Document thread-safety guarantees
- Don't assume word size, endianness, or alignment
- Avoid variable-length arrays (VLAs) - stack usage must be predictable

## Current Libraries

### data_structures
Lock-free data structures for real-time systems:
- `ring_buffer`: Single-producer single-consumer FIFO buffer
- No dynamic allocation, user-provided backing storage
- Suitable for interrupt/thread communication

### json_parser
Lightweight streaming JSON parser for embedded systems:
- Event-driven callback API (similar to SAX for XML)
- Zero-copy string parsing (pointers into original buffer)
- No dynamic memory allocation
- Configurable maximum nesting depth
- Line/column error reporting
- Suitable for parsing sensor data, config files, API responses

**Key design choices:**
- Streaming parser vs. DOM tree: Minimal memory footprint, can parse large JSON incrementally
- No escape sequence unescaping: Caller must handle `\n`, `\t`, etc. if needed
- Callback returns `bool`: Return `false` to stop parsing early (efficient value extraction)

**Thread safety:** Not thread-safe. Use separate parser instances per thread.

## Future Planned Libraries

- `networking`: TCP/IP, UDP, protocol implementations
- `math`: Linear algebra, quaternions, coordinate transforms
- `embedded_hal`: Hardware abstraction layer for peripherals
- `control`: PID controllers, filters, state machines
- `sensors`: Common sensor interfaces and drivers
