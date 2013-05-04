#ifndef __SCENE_H__
#define __SCENE_H__

#include "model.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

/* Singleton class representing the scene.
 * This contains handles to the GL resources.
 */
class Scene
{
public:
  static Scene* instance(void);

  bool setupGraphics(int w, int h);
  void renderFrame(void);

private:
  Scene(void);
  static Scene* mInstance;

  Model* theBunny;

  GLuint gProgram;
  GLuint gvPositionHandle;

  GLuint mVertexBuffer;
  GLuint mNormalBuffer;
};

#endif
