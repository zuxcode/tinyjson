cmake_minimum_required(VERSION 3.16)

project(tinyjson
    VERSION 1.0.0
    DESCRIPTION "Json parser"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Library
add_library(tinyjson_lib
    src/tokenizer.c
    # src/parser.c
    # src/json.c
)

target_include_directories(tinyjson_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Executable
add_executable(tinyjson
    src/main.c
)

target_link_libraries(tinyjson
    PRIVATE tinyjson_lib
)

# Tests
include(CTest)

if(BUILD_TESTING)
    enable_testing()

    add_executable(test_tokenizer
        tests/test_tokenizer.c
    )

    target_link_libraries(test_tokenizer
        PRIVATE tinyjson_lib
    )

    add_test(
        NAME test_tokenizers
        COMMAND test_tokenizer
    )
endif()
