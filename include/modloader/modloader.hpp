#pragma once

#include "modloader/version.hpp"

namespace modloader {

// Initializes the core runtime. Safe to call more than once.
bool initialize();

// Reports whether initialize() has completed successfully.
bool is_initialized() noexcept;

} // namespace modloader
