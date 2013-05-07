#ifndef __SCENE_H__
#define __SCENE_H__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define _GLIBCXX_USE_C99_MATH 1
#include <glm.hpp>
#include "model.h"

#include "quaternion.h"
#include "spline.h"

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
  GLuint mAttrVertexPosition;
  GLuint mAttrVertexNormal;

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


  // HACK for HW 5
Spline sX, sY, sZ; // Spline interpolation in each dimension
Quat startQuat, endQuat;

};

#endif
