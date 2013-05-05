
#include <jni.h>
#include <android/asset_manager_jni.h>
#include "log.h"
#include "gl_code.h"
#include "scene.h"
#include "jnibridge.h"

JniBridge* JniBridge::mInstance = 0;

JniBridge* JniBridge::instance()
{
  if(mInstance == NULL){
    mInstance = new JniBridge;
  }
  return(mInstance);
}

JniBridge::JniBridge()
{
  mJniEnv = 0;
  mAssetManager = 0;
}

void JniBridge::setAssetManager(JNIEnv* env, jobject assetManager)
{
  mAssetManager = AAssetManager_fromJava(env, assetManager);
}

void JniBridge::setPngLoader(JNIEnv* env, jobject pngLoader)
{
  mJniEnv = env;
  mPngLoader = env->NewGlobalRef(pngLoader);
}

void JniBridge::loadPng(const char* path)
{
  jclass cls = mJniEnv->GetObjectClass(mPngLoader);
  jmethodID mid; // Variable holding the method to call; gets reused for each call

  // Ask the PNG manager for a bitmap
  mid = mJniEnv->GetMethodID(cls, "open", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
  jstring fileName = mJniEnv->NewStringUTF(path); // Convert the path to a Java string
  jobject png = mJniEnv->CallObjectMethod(mPngLoader, mid, fileName);
  mJniEnv->DeleteLocalRef(fileName); // Delete our reference to the filename string
  mJniEnv->NewGlobalRef(png);

  // Get image dimensions
  mid = mJniEnv->GetMethodID(cls, "getWidth", "(Landroid/graphics/Bitmap;)I");
  int width = mJniEnv->CallIntMethod(mPngLoader, mid, png);
  mid = mJniEnv->GetMethodID(cls, "getHeight", "(Landroid/graphics/Bitmap;)I");
  int height = mJniEnv->CallIntMethod(mPngLoader, mid, png);
/*
		// Get pixels
		jintArray array = mJniEnv->NewIntArray(width * height);
		mJniEnv->NewGlobalRef(array);
		mid = mJniEnv->GetMethodID(cls, "getPixels", "(Landroid/graphics/Bitmap;[I)V");
		mJniEnv->CallVoidMethod(mPngLoader, mid, png, array);

		jint *pixels = mJniEnv->GetIntArrayElements(array, 0);
		*/
  LOGI("Loaded image %s of size %d x %d", path, width, height);
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
    JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_setPngLoader(JNIEnv * env, jobject obj, jobject loader);
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
    JniBridge::instance()->setAssetManager(env, am);
}

JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_setPngLoader(JNIEnv * env, jobject obj, jobject loader)
{
    JniBridge::instance()->setPngLoader(env, loader);
}

JNIEXPORT void JNICALL Java_edu_stanford_sebell_rot_GL2JNILib_touchEvent(JNIEnv * env, jobject obj,  jfloat x, jfloat y)
{
  LOGI("Touch event: %f %f", x, y);
  JniBridge::instance()->loadPng("levels/01/slice0000.png");
    //Scene::instance()->setupGraphics(width, height);
}
