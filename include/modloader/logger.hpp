#pragma once

#include <string_view>

namespace modloader {

enum class LogLevel {
    debug,
    info,
    warning,
    error
};

// Writes a message to the platform-neutral logger (stderr for now).
void log(LogLevel level, std::string_view message);

} // namespace modloader
