#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace modloader {

// Owned metadata for one registered mod. The registry stores a copy so callers
// may safely pass temporary strings during registration.
struct ModMetadata {
    std::string id;
    std::string name;
    std::string version;
};

// Registers metadata with the runtime. IDs must be unique and use only
// lowercase ASCII letters, digits, '.', '_' or '-'. Runtime must be initialized.
bool register_mod(const ModMetadata& metadata);

// Returns a snapshot; callers do not hold the registry lock while using it.
std::vector<ModMetadata> registered_mods();

// Returns a copy of the matching metadata, or nullopt when not registered.
std::optional<ModMetadata> find_mod(std::string_view id);

} // namespace modloader
