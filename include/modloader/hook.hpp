#pragma once

namespace modloader {

using HookHandle = void*;

// Initializes the Android inline-hook backend. On non-Android platforms this
// returns false because native game hooks are only supported on Android.
bool initialize_hook_backend();

// Hooks a complete native function. `original` receives the callable trampoline.
// Returns nullptr on failure. The backend must be initialized first.
HookHandle hook_function(void* target, void* replacement, void** original);

// Removes a hook previously returned by hook_function().
bool unhook_function(HookHandle handle);

} // namespace modloader
