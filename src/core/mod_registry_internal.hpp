#pragma once

#include "modloader/mod.hpp"

namespace modloader::detail {
bool unregister_mod(std::string_view id);
}
