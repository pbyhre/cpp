# EmbeddedToolchains.cmake
# Helper functions and configurations for embedded cross-compilation
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake ..

# Function to configure for bare-metal embedded targets
function(configure_bare_metal TARGET_NAME)
    # No standard library for bare metal
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TARGET_NAME} PRIVATE
            -ffreestanding
            -nostdlib
            -fno-builtin
        )
        target_link_options(${TARGET_NAME} PRIVATE
            -nostdlib
            -nostartfiles
        )
    endif()

    # Common embedded defines
    target_compile_definitions(${TARGET_NAME} PRIVATE
        BARE_METAL=1
        NO_STDLIB=1
    )
endfunction()

# Example toolchain configurations can be placed in cmake/toolchains/
# Examples:
#   - cmake/toolchains/arm-none-eabi.cmake   (ARM Cortex-M)
#   - cmake/toolchains/riscv32-unknown-elf.cmake
#   - cmake/toolchains/xtensa-esp32.cmake
