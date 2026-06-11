cmake_minimum_required(VERSION 3.20)
project(RoboticsLibs
    VERSION 0.1.0
    LANGUAGES C CXX
    DESCRIPTION "Cross-platform C/C++ libraries for embedded and robotics development")

# Project-wide options
option(BUILD_ALL_LIBS "Build all libraries" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Export compile commands for tooling (clang-tidy, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include shared CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerSettings)
include(TestFramework)

# Testing support
if(BUILD_TESTS)
    enable_testing()
    include(CTest)

    # Fetch Google Test for host-based testing
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Enable GoogleTest's test discovery
    include(GoogleTest)
endif()

# Add libraries (each library checks its own BUILD_<LIB>_LIB option)
add_subdirectory(libs/data_structures)
add_subdirectory(libs/json_parser)
# add_subdirectory(libs/networking)
# add_subdirectory(libs/math)
# add_subdirectory(libs/embedded_hal)

# Add examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
include(GNUInstallDirs)
install(EXPORT RoboticsLibsTargets
    FILE RoboticsLibsTargets.cmake
    NAMESPACE robotics::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RoboticsLibs)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/RoboticsLibsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/RoboticsLibsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RoboticsLibs)
