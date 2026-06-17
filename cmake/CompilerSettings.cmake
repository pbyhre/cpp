# CompilerSettings.cmake
# Shared compiler and linker settings for all libraries

# Function to apply common settings to a target
function(apply_common_settings TARGET_NAME)
    # Enable warnings
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
            /W4                 # High warning level
            # /WX               # Treat warnings as errors (disabled for now)
            /permissive-        # Standards conformance
        )
    else()
        target_compile_options(${TARGET_NAME} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror             # Treat warnings as errors (optional, can be disabled)
            -Wconversion
            -Wsign-conversion
            -Wshadow
        )
    endif()

    # Embedded-specific: optional features
    if(ROBOTICS_NO_EXCEPTIONS)
        if(MSVC)
            target_compile_options(${TARGET_NAME} PRIVATE /EHs-c-)
            target_compile_definitions(${TARGET_NAME} PRIVATE _HAS_EXCEPTIONS=0)
        else()
            target_compile_options(${TARGET_NAME} PRIVATE -fno-exceptions)
        endif()
    endif()

    if(ROBOTICS_NO_RTTI)
        if(MSVC)
            target_compile_options(${TARGET_NAME} PRIVATE /GR-)
        else()
            target_compile_options(${TARGET_NAME} PRIVATE -fno-rtti)
        endif()
    endif()

    # Position independent code for shared libraries
    set_target_properties(${TARGET_NAME} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endfunction()

# Embedded options
option(ROBOTICS_NO_EXCEPTIONS "Disable C++ exceptions for embedded targets" OFF)
option(ROBOTICS_NO_RTTI "Disable RTTI for embedded targets" OFF)
option(ROBOTICS_NO_HEAP "Build without dynamic memory allocation" OFF)

# Platform detection helpers
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compiling for ${CMAKE_SYSTEM_NAME} on ${CMAKE_SYSTEM_PROCESSOR}")
endif()
