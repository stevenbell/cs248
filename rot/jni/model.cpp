
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "log.h"
#include "model.h"


Model::Model(const char* filename, GLuint vertexBuf, GLuint normalBuf)
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
      printf("Unexpected input character '%c'\n", type1);
      fscanf(f, " %[^\n]", lineBuf); // Eat the whole line
    }
  }

  printf("Loaded model %s with %ld vertices, %ld faces\n", filename, mVertices.size(), mTriangles.size());

  calculateFaceNormals();
  if(mVertexNormals.size() == 0){
    calculateVertexNormals();
  }

  mVertexBuf = vertexBuf;
  mNormalBuf = normalBuf;
  mFlatShading = true;
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
void Model::loadTexture(const char* filename)
{
  // Load the bitmap using SDL
  mTextureImage = SDL_LoadBMP(filename);
  if(mTextureImage == NULL){
    printf("Error loading image %s", filename);
    return;
  }
  else{
    printf("Loaded image of size %d x %d\n", mTextureImage->w, mTextureImage->h);
  }

  // Set up textures
  glGenTextures(1, &mTextureBuf);
  glBindTexture(GL_TEXTURE_2D, mTextureBuf);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureImage->w, mTextureImage->h,
     0, GL_BGRA, GL_UNSIGNED_BYTE, mTextureImage->pixels);

  mUseTexture = true;
}
*/

void Model::render(GLuint attrVertexPosition, GLuint attrVertexNormal)
{
  GLfloat vertexArr[9]; // 3 vertices with 3 coordinates define a triangle
  GLfloat normalArr[9]; // Vertex normals

  // Iterate through all of the triangles and draw them
  // TODO: This is grossly inefficient.  Calculate all the points once,
  // put them in the vertex buffer, and then just render it over and over.
  for(int i = 0; i < mTriangles.size(); i++){
    // Get the point index from the triangle, and look up the vertex coordinates
    // Vertex indices start at zero
    vertexArr[0] = mVertices[mTriangles[i].a].x;
    vertexArr[1] = mVertices[mTriangles[i].a].y;
    vertexArr[2] = mVertices[mTriangles[i].a].z;

    vertexArr[3] = mVertices[mTriangles[i].b].x;
    vertexArr[4] = mVertices[mTriangles[i].b].y;
    vertexArr[5] = mVertices[mTriangles[i].b].z;
  
    vertexArr[6] = mVertices[mTriangles[i].c].x;
    vertexArr[7] = mVertices[mTriangles[i].c].y;
    vertexArr[8] = mVertices[mTriangles[i].c].z;

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vertexArr, GL_DYNAMIC_DRAW); // STATIC vs DYNAMIC?
    glVertexAttribPointer(attrVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if(mFlatShading){
      normalArr[0] = normalArr[3] = normalArr[6] = mFaceNormals[i].x;
      normalArr[1] = normalArr[4] = normalArr[7] = mFaceNormals[i].y;
      normalArr[2] = normalArr[5] = normalArr[8] = mFaceNormals[i].z;
    }
    else{
      //printf("norm indices: %d %d %d\n", mTriangles[i].aNorm, mTriangles[i].bNorm, mTriangles[i].cNorm);
      normalArr[0] = mVertexNormals[mTriangles[i].aNorm].x;
      normalArr[1] = mVertexNormals[mTriangles[i].aNorm].y;
      normalArr[2] = mVertexNormals[mTriangles[i].aNorm].z;
  
      normalArr[3] = mVertexNormals[mTriangles[i].bNorm].x;
      normalArr[4] = mVertexNormals[mTriangles[i].bNorm].y;
      normalArr[5] = mVertexNormals[mTriangles[i].bNorm].z;
    
      normalArr[6] = mVertexNormals[mTriangles[i].cNorm].x;
      normalArr[7] = mVertexNormals[mTriangles[i].cNorm].y;
      normalArr[8] = mVertexNormals[mTriangles[i].cNorm].z;
    }
    // printf("%f %f %f    %f %f %f\n", mFaceNormals[i].x, mFaceNormals[i].y, mFaceNormals[i].z, mVertexNormals[mTriangles[i].aNorm].x, mVertexNormals[mTriangles[i].aNorm].y, mVertexNormals[mTriangles[i].aNorm].z);

/*
    printf("[%0.3f %0.3f %0.3f   %0.3f %0.3f %0.3f    %0.3f %0.3f %0.3f ]  %0.3f %0.3f %0.3f\n", 
      vertexArr[0], vertexArr[1], vertexArr[2],
      vertexArr[3], vertexArr[4], vertexArr[5],
      vertexArr[6], vertexArr[7], vertexArr[8],
      mFaceNormals[i].x, mFaceNormals[i].y, mFaceNormals[i].z);
*/
    glBindBuffer(GL_ARRAY_BUFFER, mNormalBuf);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), normalArr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(attrVertexNormal, 3, GL_FLOAT, GL_TRUE, 0, 0); // Normalize the normals
/*
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureBuf);
    */

    glDrawArrays(GL_TRIANGLES, 0, 3);
  }
}

