#include "modloader/logger.hpp"

#include <cstdio>
#include <string_view>

#if defined(__ANDROID__)
#include <android/log.h>
#endif

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

#if defined(__ANDROID__)
android_LogPriority android_priority(LogLevel level) {
    switch (level) {
        case LogLevel::debug:   return ANDROID_LOG_DEBUG;
        case LogLevel::info:    return ANDROID_LOG_INFO;
        case LogLevel::warning: return ANDROID_LOG_WARN;
        case LogLevel::error:   return ANDROID_LOG_ERROR;
    }
    return ANDROID_LOG_DEFAULT;
}
#endif

} // namespace

void log(LogLevel level, std::string_view message) {
#if defined(__ANDROID__)
    __android_log_write(android_priority(level), "CoolModLoader", message.data());
#else
    std::fprintf(stderr, "[CoolModLoader][%s] %.*s\n",
        level_name(level),
        static_cast<int>(message.size()),
        message.data());
    std::fflush(stderr);
#endif
}

} // namespace modloader
