#ifndef __MODEL_H__
#define __MODEL_H__

/* Class representing an object model
 */

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <vector>

struct point3 {
  point3(GLfloat xx, GLfloat yy, GLfloat zz) { x = xx; y = yy; z = zz; }
  void operator+=(point3 &other){ x += other.x; y += other.y; z += other.z; }
  point3 operator/(float divisor){ return(point3(x/divisor, y/divisor, z/divisor)); }

  GLfloat x, y, z; // Floating point position in 3-space
};

struct triangle {
  triangle(GLuint _a, GLuint _b, GLuint _c) { a = _a; b = _b; c = _c; }

  GLuint a, b, c; // Indices into the triangle list
  GLuint aNorm, bNorm, cNorm; // Indices into the vertex normal list
  GLuint aTex, bTex, cTex; // Indices into texture coordinate list
};

struct Extents {
  float minX, maxX;
  float minY, maxY;
  float minZ, maxZ;
};

class Model
{
public:
  Model(const char* filename, GLuint attributeLocs[3]);
  bool loadTexture(const char* filename);

  void render();
  void toggleFlatShading() { mFlatShading = !mFlatShading; };
  Extents extents() { return mExtents; }

private:
  void calculateFaceNormals();
  void calculateVertexNormals();
  void loadVertexBuffers();

  bool mFlatShading;
  bool mUseTexture;

  // Geometry variables
  std::vector<point3> mVertices;
  std::vector<triangle> mTriangles;
  std::vector<point3> mFaceNormals;
  std::vector<point3> mVertexNormals;
  std::vector<point3> mTextureCoords;
  Extents mExtents;

  // OpenGL/shader handles
  enum BufferType {
    VERTEX,
    NORMAL,
    TEXTURE,
    INDEX,
    N_VERTEX_BUFFERS
  };

  GLuint mAttributeBuffers[N_VERTEX_BUFFERS];
  GLuint mAttributeLocations[3];
  GLuint mTextureBuf;

};

#endif

