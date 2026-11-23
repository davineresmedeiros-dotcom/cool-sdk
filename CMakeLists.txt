cmake_minimum_required(VERSION 3.22)
project(CoolModLoader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(modloader_core STATIC
    src/core/loader.cpp
    src/core/logger.cpp
)

target_include_directories(modloader_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(modloader_core PUBLIC cxx_std_20)
