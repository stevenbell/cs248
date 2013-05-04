
#include <jni.h>
#include "gl_code.h"
#include "scene.h"
#include "jnibridge.h"

void JniBridge::loadPNG(char const* path)
{
  jclass cls = g_env->GetObjectClass(g_pngmgr);
  jmethodID mid; // Variable holding the method to call; gets reused for each call

  // Ask the PNG manager for a bitmap
  mid = g_env->GetMethodID(cls, "open", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
  jstring fileName = g_env->NewStringUTF(path); // Convert the path to a Java string
  jobject png = g_env->CallObjectMethod(g_pngmgr, mid, fileName);
  g_env->DeleteLocalRef(fileName); // Delete our reference to the filename string
  g_env->NewGlobalRef(png);

  // Get image dimensions
  mid = g_env->GetMethodID(cls, "getWidth", "(Landroid/graphics/Bitmap;)I");
  int width = g_env->CallIntMethod(g_pngmgr, mid, png);
  mid = g_env->GetMethodID(cls, "getHeight", "(Landroid/graphics/Bitmap;)I");
  int height = g_env->CallIntMethod(g_pngmgr, mid, png);
/*
		// Get pixels
		jintArray array = g_env->NewIntArray(width * height);
		g_env->NewGlobalRef(array);
		mid = g_env->GetMethodID(cls, "getPixels", "(Landroid/graphics/Bitmap;[I)V");
		g_env->CallVoidMethod(g_pngmgr, mid, png, array);

		jint *pixels = g_env->GetIntArrayElements(array, 0);
		*/
}


extern "C" {
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_loadLevel(JNIEnv * env, jobject obj, jstring levelName);
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    Scene::instance()->setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_loadLevel(JNIEnv * env, jobject obj, jstring levelName)
{
    loadLevel(env->GetStringUTFChars(levelName, 0));
}

JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    Scene::instance()->renderFrame();
}
