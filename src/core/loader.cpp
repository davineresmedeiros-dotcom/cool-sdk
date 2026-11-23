#include "modloader/modloader.hpp"
#include "modloader/logger.hpp"

#include <atomic>

namespace modloader {

bool initialize() {
    static std::atomic<bool> initialized{false};
    bool expected = false;

    if (initialized.compare_exchange_strong(expected, true)) {
        log(LogLevel::info, "Core runtime initialized.");
    }

    return true;
}

} // namespace modloader
