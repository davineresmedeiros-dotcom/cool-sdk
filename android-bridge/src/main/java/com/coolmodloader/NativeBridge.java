package com.coolmodloader;

/** Java bridge loaded from Geometry Dash's own Android process. */
public final class NativeBridge {
    static {
        // coolmodloader links against ShadowHook; load its shared dependency first.
        System.loadLibrary("shadowhook");
        System.loadLibrary("coolmodloader");
    }

    private NativeBridge() {}

    /** True only if libGeometryDash.so is mapped in this process. */
    public static native boolean nativeAttachGame();
}
