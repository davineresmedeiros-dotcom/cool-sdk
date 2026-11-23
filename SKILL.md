---
name: geode-modding
description: Authoritative reference and patterns for writing Geometry Dash mods with the Geode SDK (currently 5.6.1). Use this skill whenever the user mentions Geode, GD modding, a GD class (PlayLayer, MenuLayer, GameManager, GJGameLevel, CCNode, CCSprite, etc.), modify hooks, mod.json, entry.cpp, the .geode format, Broma bindings, Tulip hooks, the Mod class, Cocos2d-x in a GD context, or shows any C++/CMake file that looks like a Geode mod, even if they don't explicitly say "Geode". Also use it for crash log analysis of Geode mods, fixing v4→v5 migration breakage, looking up exact GD method signatures or addresses, generating mod scaffolding, or answering "what's the type of X in PlayLayer". The skill ships the full Geode 5.6.1 binding headers, modify-hook headers, the public Geode API (loader/utils/ui/modify), Cocos2d-x headers, and a `find_class.py` helper — never guess a method signature or invent a Geode API call when this skill is loaded; look it up.
---

# Geode Modding (5.6.1)

This skill bundles the **real source-of-truth** for writing Geode mods: every binding header, every modify-hook header, the full public Geode API, and Cocos2d-x. **Never guess** a method signature, an enum value, an address, or an API name when this skill is available — look it up. The bindings include exact Windows/Mac/iOS/Android addresses as `@note[short]` comments, so the answer is always concrete.

## CRITICAL workflow: never grep blindly, never load entire blobs

The reference files in this skill are **concatenated blobs** (e.g. `bindings.hpp`
holds 638 individual GD class headers concatenated together — several megabytes).
Loading a blob whole blows the context budget. Always go through
`scripts/find_class.py` — it parses the `// ===== FILE: ... =====` markers
and extracts only the file you want.

```bash
# Locate a class binding (extracts just that class from the blob)
python3 scripts/find_class.py PlayLayer

# Show ONE method's declaration in a class, with surrounding context
# (also surfaces the matching modify-hook entry)
python3 scripts/find_class.py PlayLayer init

# List all binding stems containing a keyword
python3 scripts/find_class.py --search level

# Regex-grep across ALL blobs (bindings, modify-hooks, geode-api/*).
# Output format: <blob> :: <original_file>:<line>: <matched line>
python3 scripts/find_class.py --grep "WebRequest"

# Show a Geode API header by stem (Mod, Hook, Log, Popup, Task, file, web, ...)
python3 scripts/find_class.py --api Mod
python3 scripts/find_class.py --api Popup
python3 scripts/find_class.py --api web
```

**Use `--grep` whenever you don't know which class owns a method.** It scans
every blob at once and returns `blob :: original_file:line` so you can identify
both the category (bindings vs Geode API) and the original source file.

**NEVER `view` `bindings.hpp` or `modify-hooks.hpp` directly** — they are too
large. Always use the script. The only blobs small enough to read whole are
`platform.hpp`, `cpp-stl.hpp`, `external.hpp`, and the singletons in
`geode-api/top-level/`.

## Reference layout

Claude.ai limits skills to 200 files, so the original ~2000 source headers have
been **concatenated into category blobs**. Each blob contains many original
files separated by markers:

```
// ===== FILE: PlayLayer.hpp =====
... content ...
// ===== END FILE: PlayLayer.hpp =====
```

`find_class.py` is the right tool for navigating these blobs — it parses the
markers and extracts only the file you ask for. **Do NOT `view` an entire blob
file** (some are several MB); always go through the script.

```
references/
├── bindings.hpp              638 GD class declarations concatenated
│                             (PlayLayer, GameManager, GJGameLevel,
│                              CCMenuItemSpriteExtra, EditorUI, MenuLayer, ...)
│                             Includes per-platform addresses in @note[short]
│                             comments and inheritance chains.
├── modify-hooks.hpp          938 auto-generated modify-glue files concatenated.
│                             You don't edit these; they are what `class
│                             $modify(...)` resolves against. Use `find_class.py
│                             <Class> <method>` to confirm a method is hookable
│                             (presence of `GEODE_AS_STATIC_FUNCTION(method)`).
├── codegen-data/             CodegenData.txt (text dump of all class/method
│                             addresses, usable with grep) +
│                             GeneratedBinding/Modify/Predeclare.hpp.
├── geode-api/                Geode SDK public API blobs:
│   ├── loader.hpp            Mod, Hook, Event, Log, Setting, ModMetadata,
│   │                         Dirs, IPC, Tulip — concatenated.
│   ├── utils.hpp             file, web, Task, async, json, string, hash,
│   │                         base64, timer, cocos (helpers), VersionInfo,
│   │                         NodeIDs, ColorProvider, ... — concatenated.
│   ├── ui.hpp                Popup, MDPopup, MDTextArea, ListView,
│   │                         ScrollLayer, TextInput, Notification,
│   │                         ColorPickPopup, Layout, ... — concatenated.
│   ├── modify.hpp            Modify, Field, IDManager, Traits, Comparer,
│   │                         AsStaticFunction — concatenated.
│   ├── cocos.hpp             Full Cocos2d-x headers (264 files): CCNode,
│   │                         CCSprite, CCMenu, CCDirector, CCScene, CCLayer,
│   │                         actions, scheduler, ... — concatenated.
│   ├── platform.hpp          Per-OS abstractions (Mac, Win32 retained;
│   │                         Android/iOS pruned for size).
│   ├── fmod.hpp              FMOD audio headers concatenated.
│   ├── cpp-stl.hpp           gdstdlib (gd::string, gd::vector, gd::map —
│   │                         ABI-critical types). Note: in C++ source code,
│   │                         the include path you write is
│   │                         `<Geode/c++stl/gdstdlib.hpp>` (with `+`) — that
│   │                         is the upstream path the compiler expects. The
│   │                         skill stores it without `+` because Claude.ai
│   │                         disallows `+` in skill paths.
│   ├── external.hpp          Vendored externals (matjson, Result, etc.).
│   └── top-level/            Singleton top-level headers, NOT concatenated:
│                             Geode.hpp, Bindings.hpp, Loader.hpp, UI.hpp,
│                             Utils.hpp, Modify.hpp, Prelude.hpp,
│                             DefaultInclude.hpp.
└── examples/                 entry.cpp, root_CMakeLists.txt, flash.toml,
                              VERSION (5.6.1), geode_README.md,
                              geode_CHANGELOG.md.

NOTE: This skill ships HEADERS ONLY (concatenated). The generated .cpp binding
sources, the loader's internal implementation source, and platform-specific
prebuilt libraries (libcurl etc.) are NOT included. If you need to see how
Geode implements something internally, the user can point you at their local
install at `Users/Mifu/Documents/Geode/loader/src/`, or fetch from
https://github.com/geode-sdk/geode .
```

## Mod project anatomy

A minimal Geode mod is three files: `mod.json`, `CMakeLists.txt`, `src/main.cpp`.



### `mod.json` (Geode 5.x format)

**Always use these exact versions for Mifu's mods unless explicitly told otherwise:**
- `"geode": "5.6.1"` (current Geode SDK version Mifu has installed)
- `"gd"`: all four platforms set to `"2.2081"` (current GD version Mifu targets)

```json
{
    "geode": "5.6.1",
    "gd": {
        "win": "2.2081",
        "mac": "2.2081",
        "android": "2.2081",
        "ios": "2.2081"
    },
    "id": "mifu.example-mod",
    "name": "Example Mod",
    "version": "v1.0.0",
    "developer": "Mifu",
    "description": "Short description.",
    "tags": ["gameplay"]
}
```

`geode` is the **minimum** Geode version required to load the mod (use `5.6.1` to match Mifu's installed SDK). `gd` lists the GD versions per platform that the mod supports — Mifu targets `2.2081` everywhere. `id` follows `developer.modname` and is what `Mod::get()->getID()` returns. Confirm the current schema with `python3 scripts/find_class.py --api ModMetadata`.

### `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ExampleMod VERSION 1.0.0)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Define GEODE_SDK in your environment.")
endif()

include($ENV{GEODE_SDK}/cmake/Geode.cmake)
setup_geode_mod(${PROJECT_NAME})
```

### `src/main.cpp` (canonical hook example)

```cpp
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        log::info("PlayLayer initialized for level {}", level->m_levelName);
        return true;
    }
};
```

The signature **must** match the binding exactly. To verify, run:

```bash
python3 scripts/find_class.py PlayLayer init
```

If the signature differs, the hook silently won't apply or the build will fail with a `static_assert` from one of the `GEODE_APPLY_MODIFY_FOR_FUNCTION_ERROR_*` macros.

## The modify macro: how hooks work

```cpp
class $modify(MyName, BaseClass) {
    // Override any virtual or non-virtual method that exists on BaseClass.
    // Call BaseClass::method(...) inside to invoke the original.

    // Add per-instance fields (NOT regular members — they won't survive):
    struct Fields {
        int extraScore = 0;
        cocos2d::CCSprite* badge = nullptr;
    };
    // Access with m_fields->extraScore from any method.
};
```

Key rules:

- **Never declare regular member variables** in a `$modify` class. Use `struct Fields { ... };`. Geode allocates the field block separately because the original GD class has a fixed memory layout.
- **The first arg of `$modify` is a unique tag** — give it a name unique to your mod to avoid ODR collisions when multiple mods hook the same class.
- **Calling the original**: inside an override, `BaseClass::method(args)` calls the unmodified function. NOT `this->method(args)` — that recurses into your hook.
- **Adding entirely new methods** is allowed; they become callable on the instance via the modify type.
- **Static methods** are hooked the same way; just declare them `static`.

For the macro internals, run `python3 scripts/find_class.py --api Modify`. For the `Fields` mechanism, `python3 scripts/find_class.py --api Field`.

## Most-used Geode APIs

Before using any of these, run `python3 scripts/find_class.py --api <name>` to read the actual current signatures. Names below are the file stems (under `references/geode-api/Geode/`).

| Need | Header | Highlights |
|---|---|---|
| Logging | `loader/Log` | `log::debug/info/warn/error` with `fmt`-style format strings |
| Mod metadata + storage | `loader/Mod` | `Mod::get()`, `getSettingValue<T>`, `getSavedValue<T>`, `setSavedValue`, `getSaveDir`, `getResourcesDir` |
| Settings | `loader/Setting`, `loader/SettingV3` | Custom setting types via `registerCustomSettingType` |
| Events / dispatch | `loader/Event`, `loader/Dispatch` | Cross-mod event bus with type-safe filters |
| Manual hooks | `loader/Hook`, `loader/Tulip` | Use only when `$modify` doesn't fit (e.g., hooking by raw address) |
| HTTP | `utils/web` | `web::WebRequest().get(url).get(...).post(...)`, returns a `Task` |
| Filesystem | `utils/file` | `file::readString`, `file::writeBinary`, `file::pickFile`, `file::createDirectoryAll` |
| JSON | external `matjson` (via `Geode/Result.hpp` + `<matjson.hpp>`) | `matjson::parse`, `dump`, structured access |
| Async / Tasks | `utils/Task`, `utils/async` | `Task<T>` is Geode's Promise-equivalent. `.listen()` for callbacks. |
| Coroutines | `utils/coro` | C++20 coroutine integration with `Task` |
| Strings | `utils/string`, `utils/ZStringView` | UTF-8 helpers, `string::contains`, `numberToString`, etc. |
| Crypto / hashing | `utils/hash`, `utils/base64` | SHA-256, MD5, base64url |
| Timers | `utils/timer` | Lightweight scheduling helpers |
| UI popups | `ui/Popup`, `ui/MDPopup` | Inherit `Popup<Args...>`, override `setup`. `MDPopup` for markdown content. |
| UI inputs | `ui/TextInput`, `ui/ColorPickPopup`, `ui/SliderNode` | |
| Layouts | `ui/Layout`, `ui/SimpleAxisLayout` | `setLayout(AxisLayout::create(Axis::Horizontal))`, etc. |
| Notifications | `ui/Notification` | `Notification::create("Saved", NotificationIcon::Success)->show()` |
| Scrolling lists | `ui/ScrollLayer`, `ui/ListView` | |
| Scene / layer events | `ui/SceneEvent`, `ui/EnterLayerEvent` | Filter on layer ID to react when a specific layer is pushed |
| Node IDs | `utils/NodeIDs` | Stable IDs for nodes (used by NodeIDs mod conventions) |
| Cocos helpers | `utils/cocos` | Conversions, color, screen size, `getChildOfType<T>` |

## Cocos2d-x quick map

The version embedded in GD is a fork of Cocos2d-x 2.x. Common types:

- `CCNode` / `CCLayer` / `CCScene` / `CCSprite` / `CCLabelBMFont` / `CCMenu` / `CCMenuItemSpriteExtra`
- Director: `CCDirector::sharedDirector()->getWinSize()`, `pushScene`, `replaceScene`
- Actions: `CCMoveTo`, `CCFadeIn`, `CCSequence`, `CCEaseInOut`, etc. — under `geode-api/Geode/cocos/actions/`
- Scheduler: `schedule(schedule_selector(MyClass::myUpdate), 0.1f)`

To enumerate available cocos types, use `python3 scripts/find_class.py --grep "^class CC"` or look up a specific one with `--api CCNode`.

## Common gotchas

- **`TodoReturn`**: a placeholder return type for functions whose return type isn't yet known in the bindings. Trying to hook one fails to compile. Either pick another hook point or add the return type to your local copy (NOT applicable here — these bindings are read-only references).
- **"Out of line" / "Inlined" addresses**: a method marked `Inlined` in `@note[short]` is inlined by the compiler on that platform and **cannot be hooked** there. The skill's modify macro will surface this as a `static_assert` failure (`GEODE_APPLY_MODIFY_FOR_FUNCTION_ERROR_INLINE`).
- **`Rebinded` (Android)**: address resolution is delegated to the rebind table; the hook still works.
- **v4→v5 breaking changes** (Mifu's known pain point): `EventListener` filter API changed, several utility namespaces moved, `Result<>` is now from a separate `Geode/Result.hpp` (not `<Geode/utils/Result.hpp>`), `Setting` got a V3 redesign in `loader/SettingV3.hpp`. Always confirm against the v5.6.1 headers in `geode-api/`, not against memory of v4 patterns.
- **`gd::string` vs `std::string`**: GD methods that take/return strings use `gd::string` (defined in `<Geode/c++stl/gdstdlib.hpp>` — that's the include path you write in C++ source code; in this skill the file is inside `references/geode-api/cpp-stl.hpp` blob). It implicitly converts to `std::string` in most cases, but for ABI-critical args, pass `gd::string` directly.
- **Class layout / `m_*` member access**: members are at fixed offsets and the bindings name them `m_xxx`. They are public in the bindings even if private in the original. Don't add or reorder members in a `$modify`.
- **String formatting**: Geode uses `fmt::format` style, not `printf`. `log::info("x={}, y={:.2f}", x, y)` — not `%d`/`%f`.

## When asked to write a mod

1. **Default versions** — unless the user explicitly says otherwise, always set `"geode": "5.6.1"` and `"gd": { "win": "2.2081", "mac": "2.2081", "android": "2.2081", "ios": "2.2081" }`. Don't ask, don't deviate.
2. Confirm the **hook point**: which class, which method. Run `find_class.py <Class> <method>` to lock the exact signature.
3. Check the method is hookable on the user's target platform — read the `@note[short]` block. If the user is on Windows (Mifu's primary target based on PHANTOM tooling), prioritize the `Windows:` address line.
4. Write `mod.json` + `CMakeLists.txt` + `src/main.cpp`. Keep dependencies minimal.
5. Use `Mod::get()->getSavedValue<T>(key, default)` for persistence (writes to `<save dir>/saved.json`), and `Mod::get()->getSettingValue<T>(key)` for user-configurable settings declared in `mod.json`.
6. **Don't invent helper functions or namespaces** — if you reach for something like `geode::utils::doX`, verify with `--grep` first. If it doesn't exist, write the helper inline.

## When analyzing a crash log

Geode crash logs include a stack trace with `Mod` name + version per frame, the GD version, the OS, and the loaded mod list. Workflow:

1. Identify the topmost frame that belongs to a user mod (not the loader, not GD itself).
2. If that mod is one of the user's, locate the offending method via `find_class.py`.
3. Common causes: dereferencing a null result from `getChildByID`, calling a method with a stale `this` after a scene change (use `m_fields` + a CCObject ref), wrong format string args, hooking a method that doesn't exist on the target platform.

## Doing manual / non-`$modify` hooks

For hooking a function that's not in the bindings (e.g., a free function, or by raw address), use `Mod::get()->hook(...)` — see `loader/Hook.hpp` and `loader/Tulip.hpp`. This is the lower-level API the modify macro builds on.

## Don't do

- Don't fetch Geode docs from the web when this skill is loaded — the references here are 5.6.1 source-of-truth, the docs site can lag.
- Don't paste a method signature from memory; **always look it up first**.
- Don't hand-write `mod.json` schemas; mirror an existing example or read `loader/ModMetadata.hpp`.
- Don't suggest installing dependencies or running `geode build` unless the user asks — Mifu's setup is in `Users/Mifu/Documents/Geode` and is already configured.
