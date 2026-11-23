#pragma once

#include <cstdint>

namespace modloader {

// Stable, C-compatible descriptor shared by the loader and native mods.
// Keep fields as primitive types/pointers; do not pass C++ STL objects over
// the module boundary.
inline constexpr std::uint32_t mod_abi_version = 1;

struct ModDescriptor {
    std::uint32_t abi_version;
    const char* id;
    const char* name;
    const char* version;
};

using ModInfoFunction = const ModDescriptor* (*)();
using ModLoadFunction = int (*)();
using ModUnloadFunction = void (*)();

} // namespace modloader

// Every native mod must export these three functions with C linkage.
#define COOLMOD_EXPORT extern "C" __attribute__((visibility("default")))

// Example:
// COOLMOD_EXPORT const modloader::ModDescriptor* coolmodloader_mod_info() { ... }
// COOLMOD_EXPORT int coolmodloader_mod_load() { return 1; }
// COOLMOD_EXPORT void coolmodloader_mod_unload() {}
