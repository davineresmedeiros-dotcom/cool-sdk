#include "modloader/modloader.hpp"
#include "modloader/logger.hpp"

#include <string>

namespace modloader {

bool initialize() {
    log(LogLevel::info, "Core initialization started.");
    log(LogLevel::info, std::string("Version: ") + version);
    log(LogLevel::info, std::string("Target Geometry Dash: ") + target_game_version);
    log(LogLevel::info, "Core initialization completed.");
    return true;
}

} // namespace modloader
