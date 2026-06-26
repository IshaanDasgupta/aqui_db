cmake_minimum_required(VERSION 3.20)
project(aqui_db LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    xxhash
    GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
    GIT_TAG v0.8.3
)

FetchContent_MakeAvailable(xxhash)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(aqui_db ${SOURCES})

target_include_directories(aqui_db
    PRIVATE
        ${CMAKE_SOURCE_DIR}/includes
        ${xxhash_SOURCE_DIR}
)

target_sources(aqui_db PRIVATE
    ${xxhash_SOURCE_DIR}/xxhash.c
)