#include "gl_util.h"
#include "wall.h"

Wall::Wall(point3 a, point3 b, point3 c, GLfloat thickness)
{
  // First, peg the two points we know
  mVertices[0] = a.x; mVertices[1] = a.y; mVertices[2] = a.z;
  mVertices[3] = b.x; mVertices[4] = b.y; mVertices[5] = b.z;

  // Find the orthogonal vector based on the third point
  vec3 ab = b - a;
  vec3 ac = c - a;
  vec3 ortho = ac - ab / vec3::dot(ab, ab) * vec3::dot(ab, ac);
  // TODO: check for stability - ortho needs to have a norm thats "big enough".

  // Add this orthogonal vector to the two known points to make a plane
  mVertices[6] = a.x + ortho.x; mVertices[7] = a.y + ortho.y; mVertices[8] = a.z + ortho.z;
  mVertices[9] = b.x + ortho.x; mVertices[10] = b.y + ortho.y; mVertices[11] = b.z + ortho.z;

  // Save for later collision computations
  mOrigin = a;
  mLenA = ab.norm();
  mSideA = ab / mLenA;
  mLenB = ortho.norm();
  mSideB = ortho / mLenB;

  // Find the cross product to get the normal vector
  mNormal = vec3::cross(ab, ac);
  mNormal = mNormal / mNormal.norm(); // Normalize to unit length

  GLfloat normals[12];
  normals[0] = normals[3] = normals[6] = normals[9] = mNormal.x;
  normals[1] = normals[4] = normals[7] = normals[10] = mNormal.y;
  normals[2] = normals[5] = normals[8] = normals[11] = mNormal.z;

  mIndices[0] = 0; mIndices[1] = 1; mIndices[2] = 2;
  mIndices[3] = 1; mIndices[4] = 2; mIndices[5] = 3;

  GLfloat texCoords[8] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

  // Set up OpenGL stuff
  // Create buffer handles for all of our vertex attribute buffers
  glGenBuffers(4, mAttributeBuffers);

  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[VERTEX]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, mVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[NORMAL]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, normals, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[TEXTURE]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, texCoords, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mAttributeBuffers[INDEX]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*6, mIndices, GL_STATIC_DRAW);
  checkGlError("Wall::Wall finished");
}

void Wall::subrender(RenderContext c)
{
  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[VERTEX]);
  glVertexAttribPointer(c.attrVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); // Note that '3' is coords/vertex, not a count

  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[NORMAL]);
  glVertexAttribPointer(c.attrVertexNormal, 3, GL_FLOAT, GL_TRUE, 0, 0); // Normalize the normals

  // TODO: If we don't take this path, the code crashes when we try to draw
  // This is because there's nothing in the texture array, but the shader needs stuff.
  if(mUseTexture){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureBuf);

    glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[TEXTURE]);
    glVertexAttribPointer(c.attrTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mAttributeBuffers[INDEX]);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

bool Wall::collidesWith(point3 p, float distance)
{
  // The point collides with the wall if the distance is less than the
  // threshold and the projection of the point onto the wall intersects the wall
  p = p - mOrigin; // Translate to origin (i.e, work with the relative distance)

  return(fabs(vec3::dot(p, mNormal)) < distance &&
         vec3::dot(p, mSideA) > 0 && vec3::dot(p, mSideA) < mLenA &&
         vec3::dot(p, mSideB) > 0 && vec3::dot(p, mSideB) < mLenB);
}

// Return a vector in the direction a collision should prevent motion
// For a wall, this will always be in the direction of the normal
vec3 Wall::collisionNormal(point3 p)
{
  p = p - mOrigin; // Translate to origin (i.e, work with the relative distance)
  if(vec3::dot(p, mNormal) > 0){
    return(mNormal);
  }
  else{
    return(-mNormal);
  }

}
