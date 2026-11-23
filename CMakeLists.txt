cmake_minimum_required(VERSION 3.22.1)
project(CoolModLoader VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
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
    # NDK Clang enables this newer diagnostic; ShadowHook v1.0.10 treats
    # warnings as errors, so disable it only for the C dependency build.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unsafe-buffer-usage")

    include(FetchContent)
    FetchContent_Declare(shadowhook_src
        GIT_REPOSITORY https://github.com/bytedance/android-inline-hook.git
        GIT_TAG v1.0.10
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR shadowhook/src/main/cpp
    )
    FetchContent_MakeAvailable(shadowhook_src)

    # ShadowHook is a separate CMake target; root C standard settings do not
    # override its C dialect. Compile it as C99 and suppress NDK diagnostics
    # that are promoted to errors by this pinned dependency.
    # ShadowHook v1.0.10 exports C-only flags (-std=c17, -Weverything)
    # as PUBLIC; they incorrectly propagate into our C++ sources. Keep its
    # include/link interface, but stop exporting C compile settings.
    set_property(TARGET shadowhook PROPERTY INTERFACE_COMPILE_OPTIONS "")
    set_property(TARGET shadowhook PROPERTY INTERFACE_COMPILE_FEATURES "")

    target_compile_options(shadowhook PRIVATE
        -std=c99
        -Wno-declaration-after-statement
        -Wno-unsafe-buffer-usage
        -Wno-switch-default
        -Wno-c11-extensions
        -Wno-reserved-identifier
    )

    add_library(coolmodloader SHARED
        src/platform/android/android_entry.cpp
        src/platform/android/game_attach.cpp
        src/platform/android/hook_backend.cpp
    )
    target_link_libraries(coolmodloader PRIVATE modloader_core shadowhook log dl)
    set_target_properties(coolmodloader PROPERTIES OUTPUT_NAME "coolmodloader")
else()
    target_sources(modloader_core PRIVATE src/platform/host/hook_backend.cpp)
    enable_testing()
    add_executable(modloader_smoke tests/core/core_smoke.cpp)
    target_link_libraries(modloader_smoke PRIVATE modloader_core)
    add_test(NAME modloader_core_smoke COMMAND modloader_smoke)
endif()
