# ARM Cortex-M toolchain file
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake ..

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# Don't try to run executables (we're cross-compiling)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Common Cortex-M flags (can be overridden per target)
set(ARM_COMMON_FLAGS "-mthumb -ffunction-sections -fdata-sections")

# Example: Cortex-M4 with FPU
# set(CMAKE_C_FLAGS_INIT "${ARM_COMMON_FLAGS} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")
# set(CMAKE_CXX_FLAGS_INIT "${ARM_COMMON_FLAGS} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--gc-sections -specs=nosys.specs")
