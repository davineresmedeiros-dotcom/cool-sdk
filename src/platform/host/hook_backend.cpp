#include "modloader/hook.hpp"

namespace modloader {

bool initialize_hook_backend() {
    return false;
}

HookHandle hook_function(void*, void*, void**) {
    return nullptr;
}

bool unhook_function(HookHandle) {
    return false;
}

} // namespace modloader
