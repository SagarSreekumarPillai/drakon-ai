cmake_minimum_required(VERSION 3.15)
project(drakon-ai)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output binary name
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

# Add executable
add_executable(drakon ${SRC_FILES})

# Optional: warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(drakon PRIVATE -Wall -Wextra -Wpedantic)
endif()
