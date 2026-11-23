# Cool Mod Loader

Independent Android mod loader and SDK for Geometry Dash **2.2091** (arm64-v8a).

This project is **not Geode and is not based on Geode**. It is being developed as its own loader, runtime, and mod API. Technical documentation and skills may be used as references, while the architecture and implementation remain independent.

## Repository layout

- `include/modloader/` — public C++ API
- `src/` — loader implementation
- `sdk/` — mod API and example mods
- `bindings/gd-2.2091/` — version-specific game bindings
- `tests/` — tests
- `docs/` — architecture and mod-development documentation

The Android launcher will be maintained in a separate repository.

## Status

Early project skeleton. The structure and interfaces are not finalized; runtime loading, Android integration, game bindings, and hooks will be implemented in stages.

## Build

The initial CMake project is a scaffold. Android packaging and the final build workflow will be added when the runtime and launcher integration are defined.

## Reference skills

`SKILL.md` and `SKILL1.md` are retained in the repository as technical references. They are not the loader itself and do not make this project a Geode fork.
