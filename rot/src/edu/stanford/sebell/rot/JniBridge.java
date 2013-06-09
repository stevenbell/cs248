package edu.stanford.sebell.rot;

/*
 * Java-side declarations for JNI calls.
 * Steven Bell <sebell@stanford.edu>
 * 1 May 2013
 */

import android.content.res.AssetManager;

public class JniBridge {
     static {
         System.loadLibrary("gl2jni");
     }

    /**
     * @param width the current view width
     * @param height the current view height
     */
     public static native void init(int width, int height);
     public static native void loadLevel(String levelName);
     public static native int step();
     public static native void setAssetManager(AssetManager am);
     public static native void setPngLoader(AssetLoader loader);
     public static native void touchEvent(float x, float y, int action);
}