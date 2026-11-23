#include "modloader/modloader.hpp"
#include "modloader/android_bridge.hpp"
#include "modloader/hook.hpp"

#include <jni.h>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    if (!modloader::initialize_hook_backend()) {
        return JNI_ERR;
    }
    if (!modloader::initialize()) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

// Called only after the launcher has loaded the bridge inside the GD process.
// This confirms the game ELF is mapped; hooks are installed by the native SDK API.
extern "C" JNIEXPORT jboolean JNICALL
Java_com_coolmodloader_NativeBridge_nativeAttachGame(JNIEnv*, jclass) {
    return modloader::android::attach_game_process() ? JNI_TRUE : JNI_FALSE;
}
