#include "jnibridge.h"
#include "gl_util.h"
#include "object.h"

const GLfloat RenderContext::IDENTITY_MATRIX_4[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                                                      0.0f, 1.0f, 0.0f, 0.0f,
                                                      0.0f, 0.0f, 1.0f, 0.0f,
                                                      0.0f, 0.0f, 0.0f, 1.0f};
Object::Object() :
  mPosition(1.0f), mUseTexture(false)
{}

/*
 * Returns true if the texture loads and is valid, false otherwise.
 */
bool Object::loadTexture(const char* filename)
{
  mTextureImage = JniBridge::instance()->loadPng(filename);

  // Set up textures
  glGenTextures(1, &mTextureBuf);
  glBindTexture(GL_TEXTURE_2D, mTextureBuf);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

  checkGlError("setting texture params");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureImage.width, mTextureImage.height,
     0, GL_RGBA, GL_UNSIGNED_BYTE, mTextureImage.data);

  checkGlError("loadTexture - glTexImage2D");

  // TODO: error checking; return false if something fails
  mUseTexture = true;
  return mUseTexture;
}

void Object::applyGravity(const glm::vec4 gravity, const std::vector<Object*> &fixedObjects, float dt)
{
  mVelocity += gravity * dt;
  // TODO: replace all this junk with real collision detection
  mPosition[3][0] += mVelocity.x * -dt;
  if(mPosition[3][0] > 4.5){ mPosition[3][0] = 4.5; mVelocity[0] = 0; }
  if(mPosition[3][0] < -4.5){ mPosition[3][0] = -4.5; mVelocity[0] = 0; }
  mPosition[3][1] += mVelocity.y * dt;
  if(mPosition[3][1] > 4.5){ mPosition[3][1] = 4.5; mVelocity[1] = 0; }
  if(mPosition[3][1] < -4.5){ mPosition[3][1] = -4.5; mVelocity[1] = 0; }

  mPosition[3][2] = -3.0f; //+= mVelocity.z * dt;
}

void Object::render(const RenderContext c)
{
  // Do any object-specific setup?
  // For now, we're doing the position matrix outside...
  subrender(c); // Call the child's rendering method
}
