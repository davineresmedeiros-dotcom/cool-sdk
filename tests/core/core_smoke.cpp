#include "modloader/modloader.hpp"
#include "modloader/hook.hpp"

int main() {
    if (!modloader::initialize()) {
        return 1;
    }
    if (!modloader::initialize()) {
        return 2;
    }

    // Host builds intentionally expose no native game-hook backend.
    if (modloader::initialize_hook_backend()) {
        return 3;
    }
    if (modloader::hook_function(nullptr, nullptr, nullptr) != nullptr) {
        return 4;
    }
    if (modloader::unhook_function(nullptr)) {
        return 5;
    }
    return 0;
}
