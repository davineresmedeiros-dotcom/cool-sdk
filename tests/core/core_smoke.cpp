#include "modloader/modloader.hpp"
#include "modloader/hook.hpp"
#include "modloader/mod.hpp"
#include "modloader/mod_runtime.hpp"

int main() {
    if (modloader::is_initialized()) {
        return 1;
    }
    if (!modloader::initialize() || !modloader::is_initialized()) {
        return 2;
    }
    if (!modloader::initialize()) {
        return 3;
    }

    const modloader::ModMetadata sample{"sample.mod", "Sample Mod", "1.0.0"};
    if (!modloader::register_mod(sample)) {
        return 4;
    }
    if (modloader::register_mod(sample)) {
        return 5; // duplicate ID must be rejected
    }
    if (modloader::register_mod({"Bad ID", "Invalid", "1.0"})) {
        return 6; // IDs cannot contain spaces or uppercase characters
    }
    if (modloader::register_mod({"missing.version", "Missing version", ""})) {
        return 7;
    }

    const auto found = modloader::find_mod("sample.mod");
    if (!found || found->name != "Sample Mod" || found->version != "1.0.0") {
        return 8;
    }
    if (modloader::find_mod("not.registered")) {
        return 9;
    }
    const auto mods = modloader::registered_mods();
    if (mods.size() != 1 || mods.front().id != "sample.mod") {
        return 10;
    }

    // Exercise the complete native module lifecycle against a tiny test .so.
    if (!modloader::load_mod_library(SAMPLE_MOD_PATH)) {
        return 11;
    }
    if (!modloader::is_mod_loaded("sample.native")) {
        return 12;
    }
    const auto loaded = modloader::loaded_mods();
    if (loaded.size() != 1 || loaded.front().id != "sample.native") {
        return 13;
    }
    const auto native_metadata = modloader::find_mod("sample.native");
    if (!native_metadata || native_metadata->name != "Sample Native Mod") {
        return 14;
    }
    if (modloader::load_mod_library(SAMPLE_MOD_PATH)) {
        return 15; // duplicate native module must not load twice
    }
    if (!modloader::unload_mod("sample.native")) {
        return 16;
    }
    if (modloader::is_mod_loaded("sample.native")
        || modloader::find_mod("sample.native")) {
        return 17; // unload removes runtime state and metadata
    }
    if (modloader::unload_mod("sample.native")) {
        return 18; // repeated unload is rejected
    }

    // Host builds intentionally expose no native game-hook backend.
    if (modloader::initialize_hook_backend()) {
        return 19;
    }
    if (modloader::hook_function(nullptr, nullptr, nullptr) != nullptr) {
        return 20;
    }
    if (modloader::unhook_function(nullptr)) {
        return 21;
    }
    return 0;
}
