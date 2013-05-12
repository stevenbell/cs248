#ifndef __GL_UTIL_H__
#define __GL_UTIL_H__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

/* Useful reference copied from header file...
#define GL_NO_ERROR                          0x0
#define GL_INVALID_ENUM                         0x0500
#define GL_INVALID_VALUE                        0x0501
#define GL_INVALID_OPERATION                    0x0502
#define GL_STACK_OVERFLOW                       0x0503
#define GL_STACK_UNDERFLOW                      0x0504
#define GL_OUT_OF_MEMORY                        0x0505
 */

void printGLString(const char *name, GLenum s);
void checkGlError(const char* op);
GLuint loadShader(GLenum shaderType, const char* pSource);
GLuint loadShaderFromAsset(GLenum shaderType, const char* path);
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);

#endif
