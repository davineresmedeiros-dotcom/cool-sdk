# Android native integration (arm64-v8a)

CoolModLoader builds as `libcoolmodloader.so` and links to the pinned ShadowHook 1.0.10 Android inline-hook backend. ShadowHook is an independent MIT-licensed dependency; its license is preserved in `third_party/shadowhook/LICENSE`.

## Launcher contract

The launcher must arrange for the bridge to be loaded **inside the Geometry Dash process**, not just inside the launcher process. The game's process and the bridge must use the same `arm64-v8a` ABI.

Package both native libraries produced by the Android build:

- `libshadowhook.so`
- `libcoolmodloader.so`

Load `com.coolmodloader.NativeBridge` in the game process. Its static initializer loads ShadowHook first, then CoolModLoader. `JNI_OnLoad` initializes the hook backend and the loader core. Then call:

```java
boolean gamePresent = NativeBridge.nativeAttachGame();
```

`true` means `libGeometryDash.so` is mapped in the current process. It does not mean a GD method has already been hooked.

## Native hook API

The public C++ API in `include/modloader/hook.hpp` exposes:

- `initialize_hook_backend()`
- `hook_function(target, replacement, &original)`
- `unhook_function(handle)`

The target and replacement must be valid native function addresses with matching ABI/signatures. The returned `original` trampoline calls the original function. Keep hook installation on a known, version-verified GD 2.2091 address; do not reuse offsets from another GD version without validating them.

## Build

The Android CMake build fetches ShadowHook at the fixed `v1.0.10` tag. Use the repository's CI workflow or configure with the Android NDK toolchain for `arm64-v8a`. The two `.so` outputs need to be packaged by the launcher.

This bridge/backend is the native foundation only. A GD 2.2091-specific hook point and launcher-side in-process loading still need to be validated on-device before claiming full in-game integration.
