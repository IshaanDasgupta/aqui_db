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

add_executable(aqui_db
    src/main.cpp
    src/core/database.cpp
    src/core/table.cpp
    src/core/types.cpp
    src/client/command.cpp
    src/client/parser.cpp
    src/utils/to_string.cpp
)

target_include_directories(aqui_db
    PRIVATE
        ${CMAKE_SOURCE_DIR}/includes
        ${tl_expected_SOURCE_DIR}/include
)