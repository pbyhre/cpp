@echo off
REM Quick rebuild script for Windows

echo Cleaning build directory...
if exist build rmdir /s /q build

echo Configuring CMake...
cmake -S . -B build
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    exit /b %errorlevel%
)

echo Building project (Release)...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

echo Running tests...
ctest --test-dir build -C Release --output-on-failure

echo.
echo Build completed successfully!
