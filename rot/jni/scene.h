#ifndef __SCENE_H__
#define __SCENE_H__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define _GLIBCXX_USE_C99_MATH 1
#include <glm.hpp>

#include <vector>
#include "object.h"
#include "orientation.h"
#include "ui.h"

/* Singleton class representing the scene.
 * This contains handles to the GL resources.
 */
enum GameStatus {
  PLAYING,
  WON,
  LOST
};

class Scene
{
public:
  static Scene* instance(void);

  void reset(void);
  bool load(const char* path);
  bool setupGraphics(int w, int h);
  int update(void);
  void renderFrame(void);
  void touchEvent(float x, float y, int action); // TODO: move me?

private:
  Scene(void);
  void calculateDeviceRotation();
  glm::mat4 calculateCameraView(glm::vec3 cameraPosition, float aspectRatio);
  static Scene* mInstance;

  bool mGraphicsConfigured; // True once the graphics context is set up and shaders are loaded
  GameStatus mGameStatus;

  std::vector<Object*> mStaticObjects; // Things which are fixed in the world (which we can collide with)
  std::vector<Object*> mDynamicObjects; // Things which move (i.e, are updated by physics)
  Object* mFinishMarker; // Object which we tag to complete a level

  // Boundaries of the scene, used for testing death
  float mXMin, mXMax;
  float mYMin, mYMax;
  float mZMin, mZMax;

  // OpenGL stuff - this might need a better home
  GLuint gProgram;
  GLuint uiShaderProgram;

  //GLuint mVertexBuffer;
  //GLuint mNormalBuffer;

  RenderContext mContext;
  RenderContext mUiContext;

  GLuint mUniformAmbient;
  GLuint mUniformDiffuse;
  GLuint mUniformSpecular;

  glm::vec3 mCameraPosition;
  glm::mat4 mProjectionMatrix;
  glm::mat4 mModelViewMatrix;

  // For now, we have only one light.  Maybe we'll add more later.
  glm::vec3 mLightPosition;

  // This clearly isn't part of the scene...
  Orientation mOrientation;
  glm::mat4 mWorldRotation;
  glm::mat4 mDeviceAxisMapping;

  Ui* mUi; // Need a pointer since we can't init until we have a GL context

  float mAspectRatio;
  float mWidth, mHeight;
};

#endif
