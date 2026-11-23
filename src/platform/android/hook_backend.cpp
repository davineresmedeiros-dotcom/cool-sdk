#include "modloader/hook.hpp"

#include <shadowhook.h>

#include <atomic>

namespace modloader {
namespace {
std::atomic<bool> initialized{false};
}

bool initialize_hook_backend() {
    if (initialized.load(std::memory_order_acquire)) {
        return true;
    }

    const int result = shadowhook_init(SHADOWHOOK_MODE_SHARED, false);
    if (result != SHADOWHOOK_ERRNO_OK) {
        return false;
    }

    initialized.store(true, std::memory_order_release);
    return true;
}

HookHandle hook_function(void* target, void* replacement, void** original) {
    if (!initialized.load(std::memory_order_acquire) || !target || !replacement || !original) {
        return nullptr;
    }

    return shadowhook_hook_func_addr(target, replacement, original);
}

bool unhook_function(HookHandle handle) {
    if (!initialized.load(std::memory_order_acquire) || !handle) {
        return false;
    }

    return shadowhook_unhook(handle) == SHADOWHOOK_ERRNO_OK;
}

} // namespace modloader
