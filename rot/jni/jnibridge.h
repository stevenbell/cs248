#ifndef __JNI_BRIDGE_H__
#define __JNI_BRIDGE_H__

class JniBridge
{
public:
  void loadPNG(char const* path);

private:
  jobject g_pngmgr;
  JNIEnv *g_env;
};

#endif
