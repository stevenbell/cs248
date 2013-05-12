#include "jnibridge.h"
#include "gl_util.h"
#include "object.h"

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

  mUseTexture = true;
  return mUseTexture;
}
