#include "modloader/android_bridge.hpp"

#if defined(__ANDROID__)
#include <link.h>
#include <cstring>

namespace modloader::android {
namespace {

int find_geometry_dash(struct dl_phdr_info* info, size_t, void* found) {
    if (info->dlpi_name && std::strstr(info->dlpi_name, "libGeometryDash.so")) {
        *static_cast<bool*>(found) = true;
        return 1;
    }
    return 0;
}

} // namespace

bool attach_game_process() {
    bool loaded = false;
    dl_iterate_phdr(find_geometry_dash, &loaded);
    return loaded;
}

} // namespace modloader::android
#endif
