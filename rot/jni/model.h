#ifndef __MODEL_H__
#define __MODEL_H__

/* Class representing an object model
 */

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "gametypes.h"
#include "object.h"
#include <vector>

struct Extents {
  float minX, maxX;
  float minY, maxY;
  float minZ, maxZ;
};

class Model : public Object
{
public:
  Model(const char* filename);

  virtual void subrender(RenderContext c);
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

  GLuint mAttributeBuffers[N_VERTEX_BUFFERS];
};

#endif

