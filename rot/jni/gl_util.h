#ifndef __GL_UTIL_H__
#define __GL_UTIL_H__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

void printGLString(const char *name, GLenum s);
void checkGlError(const char* op);
GLuint loadShader(GLenum shaderType, const char* pSource);
GLuint loadShaderFromAsset(GLenum shaderType, const char* path);
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);

#endif
