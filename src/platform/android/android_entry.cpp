#include "modloader/modloader.hpp"
#include "modloader/android_bridge.hpp"

#include <jni.h>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    if (!modloader::initialize()) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

// The launcher should call this from its NativeBridge after loading this library
// inside the Geometry Dash process. It is a process check, not a hook installer.
extern "C" JNIEXPORT jboolean JNICALL
Java_com_coolmodloader_NativeBridge_nativeAttachGame(JNIEnv*, jclass) {
    return modloader::android::attach_game_process() ? JNI_TRUE : JNI_FALSE;
}
