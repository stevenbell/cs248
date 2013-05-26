#ifndef __SCENE_H__
#define __SCENE_H__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define _GLIBCXX_USE_C99_MATH 1
#include <glm.hpp>

#include <vector>
#include "object.h"
#include "orientation.h"

//#include "quaternion.h"
//#include "spline.h"

/* Singleton class representing the scene.
 * This contains handles to the GL resources.
 */
class Scene
{
public:
  static Scene* instance(void);

  bool load(const char* path);
  bool setupGraphics(int w, int h);
  void update(void);
  void renderFrame(void);
  void touchEvent(float x, float y); // TODO: move me?

private:
  Scene(void);
  glm::mat4 calculateCameraView(glm::vec3 cameraPosition, float aspectRatio);
  static Scene* mInstance;

  bool mGraphicsConfigured; // True once the graphics context is set up and shaders are loaded

  std::vector<Object*> mStaticObjects; // Things which are fixed in the world (which we can collide with)
  std::vector<Object*> mDynamicObjects; // Things which move (i.e, are updated by physics)

  GLuint gProgram;

  GLuint mVertexBuffer;
  GLuint mNormalBuffer;

  RenderContext mContext;

  GLuint mUniformAmbient;
  GLuint mUniformDiffuse;
  GLuint mUniformSpecular;

  glm::vec3 mCameraPosition;
  glm::mat4 mProjectionMatrix;
  glm::mat4 mModelViewMatrix;

  // This clearly isn't part of the scene...
  Orientation mOrientation;
  glm::mat4 mWorldRotation;
  glm::mat4 mDeviceAxisMapping;

  // This should live in a more general place
  const static GLfloat IDENTITY_MATRIX_4[16];

  // HACK:
  bool mDoRotation;
  float mAspectRatio;
};

#endif
