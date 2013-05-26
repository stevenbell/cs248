#ifndef __WALL_H__
#define __WALL_H__

/* Class representing walls (and ceilings and floors) in the game */
#include "gametypes.h"
#include "object.h"

class Wall : public Object
{
public:
  Wall(point3 a, point3 b, point3 c, GLfloat thickness);
  virtual void subrender(RenderContext c);

private:
  // Pointer to texture
  // Pointer to vertex buffers
  GLuint mAttributeBuffers[N_VERTEX_BUFFERS];

  float mVertices[24];
  GLushort mIndices[12];
};

#endif
