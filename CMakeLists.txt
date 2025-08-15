cmake_minimum_required(VERSION 3.15)
project(drakon-ai)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory for executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# === Core library source files ===
set(CORE_SOURCES
    src/tensor.cpp
    src/tensor_ops.cpp
    src/ops.cpp
    src/activation_functions.cpp
    src/layers.cpp
    src/optimizer.cpp
    src/dataset.cpp
    src/config.cpp
    src/utils.cpp
    src/model_loader.cpp
    src/tokenizer.cpp
    src/layers/linear.cpp
    src/layers/mlp.cpp
)

# === Legacy source files (for backward compatibility) ===
set(LEGACY_SOURCES
    src/drakon.cpp
    src/drakon_model.cpp
    src/drakon_cli.cpp
    src/backend.cpp
)

# === Main drakon CLI (legacy) ===
add_executable(drakon
    src/main.cpp
    ${LEGACY_SOURCES}
    ${CORE_SOURCES}
)

# Optional: compiler warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(drakon PRIVATE -Wall -Wextra -Wpedantic)
endif()

# === Test binary for tensor/ops (legacy) ===
add_executable(test_tensor_ops
    src/main.cpp
    ${LEGACY_SOURCES}
    ${CORE_SOURCES}
)
target_include_directories(test_tensor_ops PRIVATE ${PROJECT_SOURCE_DIR}/include)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(test_tensor_ops PRIVATE -Wall -Wextra -Wpedantic)
endif()

# === Tool: generate_dummy_weights ===
add_executable(generate_dummy_weights
    src/generate_dummy_weights.cpp
)

# === New comprehensive test executables ===

# Test tensor operations
add_executable(test_tensor
    tests/test_tensor.cpp
    ${CORE_SOURCES}
)
target_include_directories(test_tensor PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Test layers
add_executable(test_layers
    tests/test_layers.cpp
    ${CORE_SOURCES}
)
target_include_directories(test_layers PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Test optimizer
add_executable(test_optimizer
    tests/test_optimizer.cpp
    ${CORE_SOURCES}
)
target_include_directories(test_optimizer PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Test dataset
add_executable(test_dataset
    tests/test_dataset.cpp
    ${CORE_SOURCES}
)
target_include_directories(test_dataset PRIVATE ${PROJECT_SOURCE_DIR}/include)

# End-to-end test
add_executable(test_end_to_end
    tests/test_end_to_end.cpp
    ${CORE_SOURCES}
)
target_include_directories(test_end_to_end PRIVATE ${PROJECT_SOURCE_DIR}/include)

# === Training executable ===
add_executable(train
    src/train.cpp
    ${CORE_SOURCES}
)
target_include_directories(train PRIVATE ${PROJECT_SOURCE_DIR}/include)

# === Evaluation executable ===
add_executable(evaluate
    src/evaluate.cpp
    ${CORE_SOURCES}
)
target_include_directories(evaluate PRIVATE ${PROJECT_SOURCE_DIR}/include)

# === Apply warnings to all targets ===
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(test_tensor PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(test_layers PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(test_optimizer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(test_dataset PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(test_end_to_end PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(train PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(evaluate PRIVATE -Wall -Wextra -Wpedantic)
endif()

# === Python bindings (if pybind11 is available) ===
find_package(pybind11 QUIET)
if(pybind11_FOUND)
    pybind11_add_module(drakon_python
        python/drakon_python.cpp
        ${CORE_SOURCES}
    )
    target_include_directories(drakon_python PRIVATE ${PROJECT_SOURCE_DIR}/include)
endif()

# === Installation ===
install(TARGETS drakon test_tensor_ops generate_dummy_weights test_tensor test_layers test_optimizer test_dataset test_end_to_end train evaluate
        RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
