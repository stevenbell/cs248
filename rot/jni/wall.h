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
  virtual bool collidesWith(point3 p, float distance);
  virtual vec3 collisionNormal(point3 p);

private:
  // Pointer to texture
  // Pointer to vertex buffers
  GLuint mAttributeBuffers[N_VERTEX_BUFFERS];

  float mVertices[24]; // TODO: don't need these; make them local and discard
  GLushort mIndices[12];

  // Pieces which we need for computing collisions
  vec3 mOrigin;
  float mLenA, mLenB; // Lengths of the two sides
  vec3 mSideA, mSideB; // Unit vectors representing the sides
  vec3 mNormal; // Normal unit vector
};

#endif
