cmake_minimum_required(VERSION 3.15)
project(drakon-ai)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory for executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# === Source files for main drakon CLI ===
add_executable(drakon
    src/main.cpp
    src/drakon_cli.cpp
    src/drakon_model.cpp
    src/tensor.cpp
    src/ops.cpp
    src/tokenizer.cpp
    src/model_loader.cpp
)

# Optional: compiler warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(drakon PRIVATE -Wall -Wextra -Wpedantic)
endif()

# === Test binary for tensor/ops ===
add_executable(test_tensor_ops
    src/main.cpp
    src/drakon_cli.cpp
    src/tensor.cpp
    src/ops.cpp
    src/tokenizer.cpp
    src/model_loader.cpp
    src/drakon_model.cpp 
)
target_include_directories(test_tensor_ops PRIVATE ${PROJECT_SOURCE_DIR}/include)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(test_tensor_ops PRIVATE -Wall -Wextra -Wpedantic)
endif()
