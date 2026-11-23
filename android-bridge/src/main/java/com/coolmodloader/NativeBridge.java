package com.coolmodloader;

/** Java bridge that must be loaded from Geometry Dash's own Android process. */
public final class NativeBridge {
    static {
        System.loadLibrary("coolmodloader");
    }

    private NativeBridge() {}

    /** Returns true only if libGeometryDash.so is mapped in this process. */
    public static native boolean nativeAttachGame();
}
