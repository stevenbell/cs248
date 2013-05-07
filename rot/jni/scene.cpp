#include <android/log.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "gl_util.h"
#include "log.h"
#include "scene.h"

Scene* Scene::mInstance = 0;

Scene* Scene::instance(void)
{
  if(!mInstance){
    mInstance = new Scene();
  }
  return(mInstance);
}

Scene::Scene(void)
{

}

bool Scene::setupGraphics(int w, int h) {
    // Print debugging information
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    // Compile and link the shader program
    gProgram = createProgram("shaders/world.vert", "shaders/diffuse.frag");
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }

    // Retrieve handles for all the uniforms and attributes
    mUniformModelView = glGetUniformLocation(gProgram, "modelViewMatrix");
    mUniformProjection = glGetUniformLocation(gProgram, "projectionMatrix");
    mUniformLightPos = glGetUniformLocation(gProgram, "lightPosition");
    mUniformCameraPos = glGetUniformLocation(gProgram, "viewPosition");
    mUniformAmbient = glGetUniformLocation(gProgram, "ambientColor");
    mUniformDiffuse = glGetUniformLocation(gProgram, "diffuseColor");

    mAttrVertexPosition = glGetAttribLocation(gProgram, "vertexPosition");
    mAttrVertexNormal = glGetAttribLocation(gProgram, "vertexNormal");
    glEnableVertexAttribArray(mAttrVertexPosition);
    glEnableVertexAttribArray(mAttrVertexNormal);

    checkGlError("get uniform/attributes locations");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");

    GLuint attributeLocs[] = {mAttrVertexPosition, mAttrVertexNormal, 0};
    theBunny = new Model("/sdcard/rot/bunny.obj", attributeLocs);

    return true;
}

/* Set the position, size, and orientation of the model within the scene.
 * The orientation is specified in terms of euler angles.
 * The mesh is centered and scaled based on its extents.
 * Note that following GL conventions, the angles are in degrees, not radians */
glm::mat4 calculateModelView(float rotX, float rotY, float rotZ, float scale)
{
  //Extents e = model->extents();
  glm::mat4 m(1.0); // Identity matrix

  //m = glm::translate(m, -glm::vec3((e.maxX + e.minX)/2, -(e.maxY + e.minY)/2, -(e.maxZ + e.minZ)/2));
  m = glm::scale(m, glm::vec3(scale, scale, scale));

  //printf("minx: %f  maxx: %f\n", e.minX, e.maxX);

  m = glm::rotate(m, rotX, glm::vec3(1.0, 0.0, 0.0));
  m = glm::rotate(m, rotY, glm::vec3(0.0, 1.0, 0.0));
  m = glm::rotate(m, rotZ, glm::vec3(0.0, 0.0, 1.0));

  return(m);
}

glm::mat4 calculateCameraView(glm::vec3 cameraPosition, float aspectRatio)
{
  glm::mat4 m(1.0); // Identity matrix

  // Translate and then rotate the world relative to the camera
  m = glm::translate(m, -cameraPosition);
  m = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), -cameraPosition, glm::vec3(0.0f, 1.0f, 0.0f)) * m;

  // Transform scene coordinates to image coordinates
  m = glm::perspective(45.0f, aspectRatio, 0.1f, 10.0f) * m;

  return(m);
}


void Scene::renderFrame(void)
{
  static int rotation;
  rotation = (rotation + 1) % 360;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepthf(1.0f);
  checkGlError("glClearColor");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  checkGlError("glClear");

  glUseProgram(gProgram);
  checkGlError("glUseProgram");

  // Calculate the orientation and camera matrices
  mCameraPosition = glm::vec3(0.0f, 0.0f, -3.0f);
  mProjectionMatrix = calculateCameraView(mCameraPosition, 1.0f);
  mModelViewMatrix = calculateModelView(-20.0f, rotation, 0.0f, 5.0f);

  // Set up the orientation and camera projection
  glUniformMatrix4fv(mUniformModelView, 1, false, (GLfloat*)glm::value_ptr(mModelViewMatrix));
  glUniformMatrix4fv(mUniformProjection, 1, false, (GLfloat*)glm::value_ptr(mProjectionMatrix));

  // Set up the lighting
  glUniform4f(mUniformAmbient, 0.0f, 0.1f, 0.3f, 1.0f);
  glUniform4f(mUniformDiffuse, 0.8f, 0.0f, 0.0f, 1.0f);
  //glUniform4f(specularUniform, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(mUniformLightPos, -3.0f, 3.0f, 0.0f, 1.0f);
  glUniform3fv(mUniformCameraPos, 1, (GLfloat*)glm::value_ptr(mCameraPosition));
  checkGlError("set uniforms");

  // Draw the model
  theBunny->render();
  checkGlError("Render model");
}
