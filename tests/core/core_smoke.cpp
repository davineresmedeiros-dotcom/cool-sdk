#include "modloader/modloader.hpp"
#include "modloader/hook.hpp"
#include "modloader/mod.hpp"

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

    // Host builds intentionally expose no native game-hook backend.
    if (modloader::initialize_hook_backend()) {
        return 11;
    }
    if (modloader::hook_function(nullptr, nullptr, nullptr) != nullptr) {
        return 12;
    }
    if (modloader::unhook_function(nullptr)) {
        return 13;
    }
    return 0;
}
