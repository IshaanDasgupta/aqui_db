cmake_minimum_required(VERSION 3.20)
project(aqui_db LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(aqui_db
    src/main.cpp
    src/core/page_manager.cpp
)

target_include_directories(aqui_db
    PRIVATE
        ${CMAKE_SOURCE_DIR}/includes
)
