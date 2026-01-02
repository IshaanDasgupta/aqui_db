cmake_minimum_required(VERSION 3.20)
project(aqui_db LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# tl_expected
FetchContent_Declare(
    tl_expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(tl_expected)

# Abseil
FetchContent_Declare(
    absl
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20250814.1
)
FetchContent_MakeAvailable(absl)

# RE2
set(RE2_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(RE2_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(RE2_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    re2
    GIT_REPOSITORY https://github.com/google/re2.git
    GIT_TAG 2025-11-05
)
FetchContent_MakeAvailable(re2)

add_executable(aqui_db
    src/main.cpp
    src/core/database.cpp
    src/core/table.cpp
    src/core/types.cpp
    src/client/command.cpp
    src/client/parser.cpp
    src/client/lexer.cpp
    src/utils/to_string.cpp
)

target_include_directories(aqui_db
    PRIVATE
        ${CMAKE_SOURCE_DIR}/includes
        ${tl_expected_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/def
)

target_link_libraries(aqui_db
    PRIVATE
        re2::re2
)
