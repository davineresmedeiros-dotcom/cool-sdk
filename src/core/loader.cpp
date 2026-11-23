#include "modloader/modloader.hpp"
#include "modloader/logger.hpp"

#include <atomic>
#include <mutex>

namespace modloader {
namespace {
std::once_flag initialization_once;
std::atomic<bool> initialized{false};
}

bool initialize() {
    std::call_once(initialization_once, [] {
        log(LogLevel::info, "Cool Mod Loader core runtime initialized.");
        initialized.store(true, std::memory_order_release);
    });
    return is_initialized();
}

bool is_initialized() noexcept {
    return initialized.load(std::memory_order_acquire);
}

} // namespace modloader
