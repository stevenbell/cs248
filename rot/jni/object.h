#ifndef __OBJECT_H__
#define __OBJECT_H__
/* Base class for a generic object in the game */

#include "texturehandler.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class RenderContext
{


};

class Object
{
public:
  bool loadTexture(const char* filename);
  virtual void render() = 0;

protected:
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
