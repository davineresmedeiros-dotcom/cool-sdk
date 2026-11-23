#include "modloader/modloader.hpp"

#include <jni.h>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    if (!modloader::initialize()) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
