#pragma once

#include "modloader/mod.hpp"

#include <string>
#include <vector>

namespace modloader {

// Loads a native mod shared library from an explicit filesystem path.
// The library must export the entry points declared in mod_api.hpp.
bool load_mod_library(const std::string& path);

// Calls the mod's unload callback, unregisters its metadata, then closes the library.
bool unload_mod(const std::string& id);

// Unloads all successfully loaded mods. Returns false if any unload failed.
bool unload_all_mods();

// Returns a snapshot of successfully loaded mods.
std::vector<ModMetadata> loaded_mods();

// Checks whether a mod's native library is currently loaded.
bool is_mod_loaded(std::string_view id);

} // namespace modloader
