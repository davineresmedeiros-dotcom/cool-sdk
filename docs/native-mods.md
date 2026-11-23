# Native mods

CoolModLoader can load a native shared library (`.so`) through the runtime API. This is a core runtime feature; it does not require an APK or a Geometry Dash process.

## ABI

Include `modloader/mod_api.hpp` in the mod and export these functions:

- `coolmodloader_mod_info` returns a pointer to a `ModDescriptor`.
- `coolmodloader_mod_load` is the mod's `onLoad` callback. Return nonzero when initialization succeeds.
- `coolmodloader_mod_unload` is the mod's `onUnload` callback.

The descriptor must use `mod_abi_version`. The ID must be unique and contain only lowercase ASCII letters, digits, dots, underscores, or hyphens. Metadata is copied by the loader.

Example:

```cpp
#include "modloader/mod_api.hpp"

namespace {
const modloader::ModDescriptor info{
    modloader::mod_abi_version,
    "example.mod",
    "Example Mod",
    "1.0.0"
};
}

COOLMOD_EXPORT const modloader::ModDescriptor* coolmodloader_mod_info() {
    return &info;
}

COOLMOD_EXPORT int coolmodloader_mod_load() {
    // Initialize this mod.
    return 1;
}

COOLMOD_EXPORT void coolmodloader_mod_unload() {
    // Release resources owned by this mod.
}
```


## Runtime use

After `modloader::initialize()`:

```cpp
modloader::load_mod_library("/path/to/example_mod.so");
modloader::unload_mod("example.mod");
```


`load_mod_library` loads the specified file with local symbol visibility and immediate symbol resolution. The loader rejects missing exports, unsupported ABI versions, invalid metadata, duplicate IDs, and failed `onLoad` callbacks. Unload calls `onUnload` before closing the library and removing its registered metadata.

Native mods are executable code. Only load mods you trust, and build them for the same CPU ABI and compatible native runtime as the loader. Mod dependencies, automatic directory discovery, and GD-specific bindings are not implemented yet.
