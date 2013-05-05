#ifndef __JNI_BRIDGE_H__
#define __JNI_BRIDGE_H__

#include <jni.h>
#include <android/asset_manager.h>

class JniBridge
{
public:
  // Singleton access method
  static JniBridge* instance();

  // These methods are invoked from the Java side to set up the connection
  void setAssetManager(JNIEnv* env, jobject assetManager);
  void setPngLoader(JNIEnv* env, jobject pngLoader);

  // These methods are invoked from the C++ side to retrieve things from the JVM
  void loadPng(const char* path);
  void loadText(const char* path, char** text);

private:
  JniBridge();
  static JniBridge* mInstance;

  JNIEnv *mJniEnv;
  AAssetManager* mAssetManager;
  jobject mPngLoader;
};

#endif
