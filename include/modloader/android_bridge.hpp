#pragma once

#if defined(__ANDROID__)
#include <jni.h>

namespace modloader::android {

// Returns true only when Geometry Dash's native library is mapped in this process.
bool attach_game_process();

} // namespace modloader::android
#endif
