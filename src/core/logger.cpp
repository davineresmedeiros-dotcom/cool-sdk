#include "modloader/logger.hpp"

#include <cstdio>
#include <string_view>

namespace modloader {
namespace {

const char* level_name(LogLevel level) {
    switch (level) {
        case LogLevel::debug:   return "DEBUG";
        case LogLevel::info:    return "INFO";
        case LogLevel::warning: return "WARN";
        case LogLevel::error:   return "ERROR";
    }
    return "UNKNOWN";
}

} // namespace

void log(LogLevel level, std::string_view message) {
    std::fprintf(stderr, "[CoolModLoader][%s] %.*s\n",
        level_name(level),
        static_cast<int>(message.size()),
        message.data());
    std::fflush(stderr);
}

} // namespace modloader
