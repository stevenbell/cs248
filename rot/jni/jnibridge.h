#ifndef __JNI_BRIDGE_H__
#define __JNI_BRIDGE_H__

#include <jni.h>
#include <android/asset_manager.h>

class JniBridge
{
public:
  static void setAssetManager(JNIEnv* env, jobject assetManager);
  void loadPNG(const char* path);
  static void loadText(const char* path, char** text);

private:
  jobject g_pngmgr;

  static AAssetManager* mAssetManager;

  JNIEnv *jEnv;
};

#endif
