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

  bool setupGraphics(int w, int h);
  void renderFrame(void);
  void touchEvent(float x, float y); // TODO: move me?

private:
  Scene(void);
  glm::mat4 calculateCameraView(glm::vec3 cameraPosition, float aspectRatio);
  static Scene* mInstance;

  std::vector<Object*> mObjects;

  GLuint gProgram;
  GLuint mAttrVertexPosition;
  GLuint mAttrVertexNormal;
  GLuint mAttrTexCoord;

  GLuint mVertexBuffer;
  GLuint mNormalBuffer;

  // Shader uniform handles
  GLuint mUniformModelView;
  GLuint mUniformProjection;
  GLuint mUniformLightPos;
  GLuint mUniformCameraPos;

  GLuint mUniformAmbient;
  GLuint mUniformDiffuse;
  GLuint mUniformSpecular;

  glm::vec3 mCameraPosition;
  glm::mat4 mProjectionMatrix;
  glm::mat4 mModelViewMatrix;

  // This clearly isn't part of the scene...
  Orientation mOrientation;
  float mAspectRatio;
};

#endif
