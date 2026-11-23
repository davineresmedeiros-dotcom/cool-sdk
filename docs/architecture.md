# Architecture — Cool Mod Loader

## Project boundaries

Cool Mod Loader is an independent mod loader for Geometry Dash 2.2091 on Android arm64-v8a. It is not Geode and is not a Geode fork. The Geode skill may be consulted as technical documentation for game classes, Cocos2d-x, and hook concepts, but the loader's design, runtime, mod format, and API are owned by this project.

The Android launcher is planned as a separate repository. This repository contains the loader core, runtime integration, public SDK, and version-specific bindings.

## Main components

### Core
Owns initialization, version information, logging, and shared services. It should not depend on UI/launcher code.

### Mod manager
Discovers mod directories, reads each `mod.json`, validates metadata and compatibility, loads the mod library, and manages its lifecycle. A mod should be isolated from other mods as far as the native runtime permits.

### Public SDK
Defines the supported interface for mod authors. Mods should use the public SDK rather than depend on internal loader implementation details.

### Hook system
Provides a controlled interface for intercepting game functions. Hook backend details stay behind an internal abstraction. Exact symbols, offsets, and signatures must be verified against the target game build; do not guess them.

### Game bindings
Contains version-specific declarations and verified addresses/signatures for Geometry Dash 2.2091. Keep these separate from generic loader code so future game versions can be added without rewriting the core.

### Android platform layer
Handles Android-specific library loading, paths, logging integration, and runtime setup. Launcher UI and installation flow remain outside this repository.

### Event system
Provides loader lifecycle and game-related events through the public SDK when those events are implemented and verified.

## Mod package draft

A mod directory is planned to contain a manifest and native library:

```text
mods/
└── example-mod/
    ├── mod.json
    └── libexample_mod.so
```

Draft manifest:

```json
{
  "id": "example.mod",
  "name": "Example Mod",
  "version": "1.0.0",
  "loader": "0.1.0",
  "game": "2.2091",
  "library": "libexample_mod.so"
}
```

The manifest format is a draft and may change before the SDK stabilizes.

## Development sequence

1. Core initialization, versioning, and logging.
2. Manifest parsing and mod discovery.
3. Public mod lifecycle/API and a minimal example mod.
4. Android runtime integration.
5. Verified GD 2.2091 bindings.
6. Hook backend and safe registration interface.
7. Events and additional SDK services.
8. Integration with the separately developed launcher.

## Safety and correctness requirements

- Do not hardcode unverified game offsets or function signatures.
- Keep target-version data in `bindings/gd-2.2091/`.
- Fail clearly when the game build is unsupported.
- Preserve the repository's license and credits when adapting any code.
- Keep reference skill files; they are documentation, not implementation dependencies.
