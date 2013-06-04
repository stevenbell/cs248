
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "gl_util.h"
#include "log.h"
#include "jnibridge.h"
#include "model.h"


Model::Model(const char* filename)
{
  char lineBuf[1024];

  FILE* f = fopen(filename, "r");

  // Initialize the extents - hopefully 1e10 is big enough!
  mExtents.minX = 1e10;
  mExtents.maxX = -1e10;
  mExtents.minY = 1e10;
  mExtents.maxY = -1e10;
  mExtents.minZ = 1e10;
  mExtents.maxZ = -1e10;

  // Read the list of points
  while(!feof(f)){
    // Read the first character, ignoring whitespace
    char type1, type2;
    fscanf(f, " %c%c", &type1, &type2);
    if(type1 == 'v' && type2 == ' '){ // Vertex
      float x, y, z;
      fscanf(f, "%f %f %f", &x, &y, &z); // Read the three coordinates
      mVertices.push_back(point3(x, y, z)); // Save them in the vector
      if(x < mExtents.minX){ mExtents.minX = x; }
      if(y < mExtents.minY){ mExtents.minY = y; }
      if(z < mExtents.minZ){ mExtents.minZ = z; }
      if(x > mExtents.maxX){ mExtents.maxX = x; }
      if(y > mExtents.maxY){ mExtents.maxY = y; }
      if(z > mExtents.maxZ){ mExtents.maxZ = z; }
    }
    else if(type1 == 'v' && type2 == 'n'){ // Vertex normal
      float x, y, z;
      fscanf(f, "%f %f %f", &x, &y, &z);
      mVertexNormals.push_back(point3(x, y, z));
    }
    else if(type1 == 'v' && type2 == 't'){ // Texture coordinate
      float x, y, z;
      fscanf(f, "%f %f %f", &x, &y, &z);
      mTextureCoords.push_back(point3(x, y, z));
    }
    else if(type1 == 'f'){ // Face
      // This could be specified as either "f A B C" or as "f A/Anorm/Atex B/Bnorm/Btex ..."
      fscanf(f, " %[^\n]", lineBuf);
      if(strchr(lineBuf, '/')){
        unsigned int a, aNorm, aTex, b, bNorm, bTex, c, cNorm, cTex;
        sscanf(lineBuf, "%d/%d/%d %d/%d/%d %d/%d/%d", 
                        &a, &aNorm, &aTex, &b, &bNorm, &bTex, &c, &cNorm, &cTex);
        mTriangles.push_back(triangle(a-1, b-1, c-1));
      }
      else{
        unsigned int a, b, c;
        sscanf(lineBuf, "%d %d %d", &a, &b, &c); // Read the three indices
        mTriangles.push_back(triangle(a-1, b-1, c-1)); // Save them in the vector
      }
    }
    else if(type1 == '#'){
      // Comment; eat up the whole line
      fscanf(f, " %[^\n]", lineBuf);
    }
    else{
      LOGI("Unexpected input character '%c' while reading OBJ file %s\n", type1, filename);
      fscanf(f, " %[^\n]", lineBuf); // Eat the whole line
    }
  }

  LOGI("Loaded model %s with %ld vertices, %ld faces\n", filename, mVertices.size(), mTriangles.size());

  calculateFaceNormals();
  if(mVertexNormals.size() == 0){
    calculateVertexNormals();
  }

  // Create buffer handles for all of our vertex attribute buffers
  glGenBuffers(4, mAttributeBuffers);

  loadVertexBuffers();

  mFlatShading = true;
  mUseTexture = false; // Not ready to use texture until we've loaded one
}

/* Calculate face normals using the vertices of each triangle.
 * This creates a vector of the same length as the triangles vector. */
void Model::calculateFaceNormals()
{
  // Calculate the normal vector for every triangle by taking the vectors
  // formed by two edges and finding their cross product
  for(int i = 0; i < mTriangles.size(); i++){
    point3 a = mVertices[mTriangles[i].a];
    point3 b = mVertices[mTriangles[i].b];
    point3 c = mVertices[mTriangles[i].c];

    float x = (a.y - b.y) * (a.z - c.z) - (a.y - c.y) * (a.z - b.z);
    float y = (a.x - c.x) * (a.z - b.z) - (a.x - b.x) * (a.z - c.z);
    float z = (a.x - b.x) * (a.y - c.y) - (a.x - c.x) * (a.y - b.y);
    float norm = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    mFaceNormals.push_back(point3(x/norm, y/norm, z/norm));
  }
}

/* Calculate vertex normals by averaging existing face normals */
void Model::calculateVertexNormals()
{
  // Iterate through all the vertices
  for(int i = 0; i < mVertices.size(); i++){
    // Find any triangles which use this vertex
    point3 average(0, 0, 0);
    int adjoiningCount = 0;
    for(int j = 0; j < mTriangles.size(); j++){
      // If any of the vertices are this vertex, then add the face normal
      // to the accumulator, and save the index in the appropriate spot.
      if(mTriangles[j].a == i){ // Assumes zero-based vertex indexing
        average += mFaceNormals[j];
        adjoiningCount++;
        mTriangles[j].aNorm = mVertexNormals.size();
      }
      if(mTriangles[j].b == i){
        average += mFaceNormals[j];
        adjoiningCount++;
        mTriangles[j].bNorm = mVertexNormals.size();
      }
      if(mTriangles[j].c == i){
        average += mFaceNormals[j];
        adjoiningCount++;
        mTriangles[j].cNorm = mVertexNormals.size();
      }
    }
    mVertexNormals.push_back(average / adjoiningCount);
  }
}

/*
 * Put the geometry data into the vertex buffers on the GPU.
 */
void Model::loadVertexBuffers()
{
  int arrSize = sizeof(float) * 3 * mVertices.size();
  float* vertexArr = (float*)malloc(arrSize);
  float* normalArr = (float*)malloc(arrSize);
  // TODO: texture coordinates

  for(int i = 0; i < mVertices.size(); i++){
    vertexArr[3*i + 0] = mVertices[i].x;
    vertexArr[3*i + 1] = mVertices[i].y;
    vertexArr[3*i + 2] = mVertices[i].z;
    normalArr[3*i + 0] = mVertexNormals[i].x;
    normalArr[3*i + 1] = mVertexNormals[i].y;
    normalArr[3*i + 2] = mVertexNormals[i].z;
  }

  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[VERTEX]);
  glBufferData(GL_ARRAY_BUFFER, arrSize, vertexArr, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[NORMAL]);
  glBufferData(GL_ARRAY_BUFFER, arrSize, normalArr, GL_STATIC_DRAW);

  checkGlError("Bind/fill vertices and normals");

  // TODO: Do this right, and use real texture coordinates
  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[TEXTURE]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*mVertices.size(), vertexArr, GL_STATIC_DRAW);

  int indexSize = sizeof(GLushort) * 3 * mTriangles.size();
  GLushort* indexArr = (GLushort*)malloc(indexSize);
  for(int i = 0; i < mTriangles.size(); i++){
    indexArr[3*i + 0] = mTriangles[i].a;
    indexArr[3*i + 1] = mTriangles[i].b;
    indexArr[3*i + 2] = mTriangles[i].c;
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mAttributeBuffers[INDEX]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexArr, GL_STATIC_DRAW);

  checkGlError("Done filling attrib arrays");
  // The data is now on the GPU, so we don't need it anymore
  free(vertexArr);
  free(normalArr);
  free(indexArr);
}


void Model::subrender(RenderContext c)
{
  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[VERTEX]);
  glVertexAttribPointer(c.attrVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0); // Note that '3' is coords/vertex, not a count
    
  glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[NORMAL]);
  glVertexAttribPointer(c.attrVertexNormal, 3, GL_FLOAT, GL_TRUE, 0, 0); // Normalize the normals

  // TODO: don't hardcode this
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureBuf);

    glBindBuffer(GL_ARRAY_BUFFER, mAttributeBuffers[TEXTURE]);
    glVertexAttribPointer(c.attrTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mAttributeBuffers[INDEX]);
  glDrawElements(GL_TRIANGLES, 3 * mTriangles.size(), GL_UNSIGNED_SHORT, 0);
}

bool Model::collidesWith(point3 p, float distance){
  // TODO use extents, at least
  if(fabs(p.x - mPosition[3][0]) < distance &&
     fabs(p.y - mPosition[3][1]) < distance &&
     fabs(p.z - mPosition[3][2]) < distance) {
    return true;
  }
  else{
    return false;
  }
}
