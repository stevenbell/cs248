#ifndef __OBJECT_H__
#define __OBJECT_H__
/* Base class for a generic object in the game */

#include "texturehandler.h"
#include "gametypes.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <glm.hpp>
#include <vector>

struct RenderContext
{
  // Shader uniform handles
  GLuint uniformModelView;
  GLuint uniformProjection;
  GLuint uniformLightPos;
  GLuint uniformCameraPos;

  // Shader attribute handles
  GLuint attrVertexPosition;
  GLuint attrVertexNormal;
  GLuint attrTexCoord;

  const static GLfloat IDENTITY_MATRIX_4[16];
};

class Object
{
public:
  Object();
  virtual ~Object();

  bool loadTexture(const char* filename);
  const glm::mat4& positionMatrix() { return mPosition; };
  void applyGravity(const glm::vec3 gravity, const std::vector<Object*> &fixedObjects, const float dt);
  void setPosition(float x, float y, float z);
  void setRotation(float x, float y, float z);
  void render(RenderContext c);

  virtual bool collidesWith(vec3 p, float distance){ return false; };
  bool collidesWith(glm::vec3 p, float distance){ return collidesWith(vec3(p.x, p.y, p.z), distance); };
  virtual vec3 collisionNormal(point3 p) = 0; //{ return vec3(1.0f, 0.0f, 0.0f); };
  glm::vec3 collisionNormal(glm::vec3 p);

protected:
  virtual void subrender(RenderContext c) = 0;

  // These things apply only to dynamic objects and might be better off in a
  // subclass or interface
  glm::mat4 mPosition; // Position and orientation of the object (model view matrix)
  glm::vec3 mVelocity;

  Image mTextureImage;
  bool mUseTexture;
  GLuint mTextureBuf;

  // OpenGL/shader handles
  enum BufferType {
    VERTEX,
    NORMAL,
    TEXTURE,
    INDEX,
    N_VERTEX_BUFFERS
  };

};

#endif
