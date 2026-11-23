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
set_target_properties(modloader_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(ANDROID)
    add_library(coolmodloader SHARED
        src/platform/android/android_entry.cpp
        src/platform/android/game_attach.cpp
    )
    target_link_libraries(coolmodloader PRIVATE modloader_core log dl)
    set_target_properties(coolmodloader PROPERTIES OUTPUT_NAME "coolmodloader")
else()
    enable_testing()
    add_executable(modloader_smoke tests/core/core_smoke.cpp)
    target_link_libraries(modloader_smoke PRIVATE modloader_core)
    add_test(NAME modloader_core_smoke COMMAND modloader_smoke)
endif()
