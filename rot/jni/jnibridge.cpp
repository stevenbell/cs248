
#include <jni.h>
#include <android/asset_manager_jni.h>
#include "log.h"
#include "gl_code.h"
#include "scene.h"
#include "jnibridge.h"

AAssetManager* JniBridge::mAssetManager = 0;

void JniBridge::setAssetManager(JNIEnv* env, jobject assetManager)
{
  mAssetManager = AAssetManager_fromJava(env, assetManager);
}

void JniBridge::loadPNG(const char* path)
{
  jclass cls = jEnv->GetObjectClass(g_pngmgr);
  jmethodID mid; // Variable holding the method to call; gets reused for each call

  // Ask the PNG manager for a bitmap
  mid = jEnv->GetMethodID(cls, "open", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
  jstring fileName = jEnv->NewStringUTF(path); // Convert the path to a Java string
  jobject png = jEnv->CallObjectMethod(g_pngmgr, mid, fileName);
  jEnv->DeleteLocalRef(fileName); // Delete our reference to the filename string
  jEnv->NewGlobalRef(png);

  // Get image dimensions
  mid = jEnv->GetMethodID(cls, "getWidth", "(Landroid/graphics/Bitmap;)I");
  int width = jEnv->CallIntMethod(g_pngmgr, mid, png);
  mid = jEnv->GetMethodID(cls, "getHeight", "(Landroid/graphics/Bitmap;)I");
  int height = jEnv->CallIntMethod(g_pngmgr, mid, png);
/*
		// Get pixels
		jintArray array = jEnv->NewIntArray(width * height);
		jEnv->NewGlobalRef(array);
		mid = jEnv->GetMethodID(cls, "getPixels", "(Landroid/graphics/Bitmap;[I)V");
		jEnv->CallVoidMethod(g_pngmgr, mid, png, array);

		jint *pixels = jEnv->GetIntArrayElements(array, 0);
		*/
}

/* Loads a text string from an APK resource
 * @param path  Path to the resource
 * @param text  Pointer to a pointer which will contain a reference to the
 * text.  The user should free the memory when finished with the text buffer.
 */

void JniBridge::loadText(const char* path, char** text)
{
  if(!mAssetManager){
    LOGE("JniBridge::loadText failed!  mAssetManager is NULL");
    return;
  }

  // Open the file and read the text
  AAsset* file = AAssetManager_open(mAssetManager, path, AASSET_MODE_BUFFER); // TODO: what's the right mode here? STREAMING?
  const char* shaderText = (char*) AAsset_getBuffer(file);
  int nBytes = AAsset_getLength(file);

  // Copy the text to the caller, and append the all-important \0
  *text = (char*) malloc(nBytes + 1);
  memcpy(*text, shaderText, nBytes);
  (*text)[nBytes] = '\0';

  AAsset_close(file); // TODO: does this free the buffer?
}

extern "C" {
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_loadLevel(JNIEnv * env, jobject obj, jstring levelName);
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_step(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_setAssetManager(JNIEnv * env, jobject obj, jobject am);
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_touchEvent(JNIEnv * env, jobject obj,  jfloat x, jfloat y);
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

JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_setAssetManager(JNIEnv * env, jobject obj, jobject am)
{
    JniBridge::setAssetManager(env, am);
}

JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_touchEvent(JNIEnv * env, jobject obj,  jfloat x, jfloat y)
{
  LOGI("Touch event: %f %f", x, y);
    //Scene::instance()->setupGraphics(width, height);
}
