#ifndef __GAMETYPE_H__
#define __GAMETYPE_H__

/* Definitions of basic useful types in the game */

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cmath>


struct point3 {
  point3(GLfloat xx, GLfloat yy, GLfloat zz) { x = xx; y = yy; z = zz; }
  void operator+=(point3 &other){ x += other.x; y += other.y; z += other.z; }
  void operator-=(point3 &other){ x -= other.x; y -= other.y; z -= other.z; }

  point3 operator+(point3 addend){ return(point3(x+addend.x, y+addend.y, z+addend.z)); }
  point3 operator-(point3 subtend){ return(point3(x-subtend.x, y-subtend.y, z-subtend.z)); }

  point3 operator*(float mul){ return(point3(x*mul, y*mul, z*mul)); }
  point3 operator/(float divisor){ return(point3(x/divisor, y/divisor, z/divisor)); }

  // Things that really apply to vectors...
  float norm(){ return(sqrt(x * x + y * y + z * z)); };
  static float dot(point3 a, point3 b){ return(a.x * b.x + a.y * b.y + a.z * b.z); };


  GLfloat x, y, z; // Floating point position in 3-space
};

typedef point3 vec3; // Just an alias for notational clarity

struct triangle {
  triangle(GLuint _a, GLuint _b, GLuint _c) { a = _a; b = _b; c = _c; }

  GLuint a, b, c; // Indices into the triangle list
  GLuint aNorm, bNorm, cNorm; // Indices into the vertex normal list
  GLuint aTex, bTex, cTex; // Indices into texture coordinate list
};


#endif
