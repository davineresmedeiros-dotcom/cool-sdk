#include "modloader/mod_api.hpp"

namespace {
bool active = false;
}

namespace modloader {
namespace {
const ModDescriptor descriptor{
    mod_abi_version,
    "sample.native",
    "Sample Native Mod",
    "1.0.0"
};
}
}

COOLMOD_EXPORT const modloader::ModDescriptor* coolmodloader_mod_info() {
    return &modloader::descriptor;
}

COOLMOD_EXPORT int coolmodloader_mod_load() {
    if (active) {
        return 0;
    }
    active = true;
    return 1;
}

COOLMOD_EXPORT void coolmodloader_mod_unload() {
    active = false;
}
