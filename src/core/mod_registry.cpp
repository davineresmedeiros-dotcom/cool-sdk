#include "modloader/mod.hpp"
#include "modloader/modloader.hpp"
#include "modloader/logger.hpp"

#include <mutex>
#include <unordered_map>
#include <utility>

namespace modloader {
namespace {

std::mutex registry_mutex;
std::unordered_map<std::string, ModMetadata> registry;

bool valid_id(std::string_view id) {
    if (id.empty()) {
        return false;
    }
    for (const unsigned char ch : id) {
        const bool valid = (ch >= 'a' && ch <= 'z')
            || (ch >= '0' && ch <= '9')
            || ch == '.' || ch == '_' || ch == '-';
        if (!valid) {
            return false;
        }
    }
    return true;
}

} // namespace

bool register_mod(const ModMetadata& metadata) {
    if (!is_initialized()
        || !valid_id(metadata.id)
        || metadata.name.empty()
        || metadata.version.empty()) {
        log(LogLevel::warning, "Rejected mod registration: invalid metadata or runtime not initialized.");
        return false;
    }

    std::lock_guard<std::mutex> lock(registry_mutex);
    const auto [it, inserted] = registry.emplace(metadata.id, metadata);
    if (!inserted) {
        log(LogLevel::warning, "Rejected mod registration: duplicate mod ID.");
        return false;
    }
    log(LogLevel::info, "Mod metadata registered.");
    return true;
}

std::vector<ModMetadata> registered_mods() {
    std::lock_guard<std::mutex> lock(registry_mutex);
    std::vector<ModMetadata> result;
    result.reserve(registry.size());
    for (const auto& entry : registry) {
        result.push_back(entry.second);
    }
    return result;
}

std::optional<ModMetadata> find_mod(std::string_view id) {
    std::lock_guard<std::mutex> lock(registry_mutex);
    const auto it = registry.find(std::string(id));
    if (it == registry.end()) {
        return std::nullopt;
    }
    return it->second;
}

} // namespace modloader
