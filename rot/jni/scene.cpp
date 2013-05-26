#include <android/log.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "model.h"
#include "wall.h"

#include "gl_util.h"
#include "log.h"
#include "scene.h"

Scene* Scene::mInstance = 0;

const GLfloat Scene::IDENTITY_MATRIX_4[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                                              0.0f, 1.0f, 0.0f, 0.0f,
                                              0.0f, 0.0f, 1.0f, 0.0f,
                                              0.0f, 0.0f, 0.0f, 1.0f};

Scene* Scene::instance(void)
{
  if(!mInstance){
    mInstance = new Scene();
  }
  return(mInstance);
}

Scene::Scene(void)
  : mGraphicsConfigured(false)
{
  mWorldRotation = glm::mat4(1.0f); // Identity matrix
  /* Hrm. This matrix looks familiar...
  mWorldRotation = glm::mat4(-1.0f, 0.0f,  0.0f, 0.0f,
                              0.0f, 0.0f,  -1.0f, 0.0f,
                              0.0f, 1.0f,  0.0f, 0.0f,
                              0.0f, 0.0f,  0.0f, 1.0f);
                              */

  // Use this for a "portrait mode" mapping
  // This makes the GL axes match the world axes, but that's not actually what we want.
  /*
  mDeviceAxisMapping = glm::mat4( 0.0f, 1.0f, 0.0f, 0.0f,
                                  1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f);
                                  */
  mDeviceAxisMapping = glm::mat4( 1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, -1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f);
  /*
  FILE* f = fopen("/sdcard/rot/movement.spline", "r");
  float t = 0.0f;
  while(!feof(f)){
    // Read each line of the file and append to the spline
    float x, y, z;
    fscanf(f, "%f %f %f", &x, &y, &z);
    sX.addKeypoint(t, x);
    sY.addKeypoint(t, y);
    sZ.addKeypoint(t, z);
    t += 1.0;
  }

  // HACK: hard-code the quaternions here
  // Leave startQuat alone
  endQuat = endQuat.rotate(glm::vec3(1.0f, 1.0f, 0.2f), 2.0f);
  */

    mCameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
}


bool Scene::setupGraphics(int w, int h) {
    // Print debugging information
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    // Compile and link the shader program
    gProgram = createProgram("shaders/world.vert", "shaders/dumbtexture.frag");
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }

    // Retrieve handles for all the uniforms and attributes
    mContext.uniformModelView = glGetUniformLocation(gProgram, "modelViewMatrix");
    mContext.uniformProjection = glGetUniformLocation(gProgram, "projectionMatrix");
    mContext.uniformLightPos = glGetUniformLocation(gProgram, "lightPosition");
    mContext.uniformCameraPos = glGetUniformLocation(gProgram, "viewPosition");
    mUniformAmbient = glGetUniformLocation(gProgram, "ambientColor");
    mUniformDiffuse = glGetUniformLocation(gProgram, "diffuseColor");

    mContext.attrVertexPosition = glGetAttribLocation(gProgram, "vertexPosition");
    mContext.attrVertexNormal = glGetAttribLocation(gProgram, "vertexNormal");
    mContext.attrTexCoord = glGetAttribLocation(gProgram, "vertexTexCoord");
    glEnableVertexAttribArray(mContext.attrVertexPosition);
    glEnableVertexAttribArray(mContext.attrVertexNormal);
    glEnableVertexAttribArray(mContext.attrTexCoord);

    checkGlError("get uniform/attributes locations");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");

    mAspectRatio = (float)w / h;

    mGraphicsConfigured = true;
    return true;
}

/* Set the position, size, and orientation of the model within the scene.
 * The orientation is specified in terms of euler angles.
 * The mesh is centered and scaled based on its extents.
 * Note that following GL conventions, the angles are in degrees, not radians */
/*
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
*/

glm::mat4 Scene::calculateCameraView(glm::vec3 cameraPosition, float aspectRatio)
{
  glm::mat4 m(1.0); // Identity matrix

  // Translate and then rotate the world relative to the camera
  m = glm::translate(m, glm::vec3(0.0f, 0.0f, 0.0f)); // TODO: replace with mCameraPosition

  glm::mat4 r = mOrientation.getCameraOrientation();

  // The device axes are specified to be +X is right, +Y is top, and +Z is out the screen
  // World axes are +X is east, +Y is north, and +Z is toward the sky.

  // To produce the viewing axis, I want the components to the world matrix in the
  // direction of -Z (into the screen).  This is the third row of the matrix.
  glm::vec4 earthLookPoint = glm::transpose(r) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
  //LOGI("earth look point: %f %f %f", earthLookPoint.x, earthLookPoint.y, earthLookPoint.z);

  // Now we have to match the real world coordinates to the GL world coordinates.

  // The world rotation matrix represents the current rotation of the world,
  // which is only modified when the device is "twisted" around the device
  // Z-axis.  The world rotation is used to calculate the direction of gravity

  // When the device rotates, we can get the Z-axis twist from the gyro
  // The view axis is given by the orientation
  // So we're just rotating a matrix by an axis and angle

  // Swizzle the world coordinates to match the GL scene coordinates
  //glm::vec4 sceneLookPoint = glm::vec4(-earthLookPoint.x, -earthLookPoint.z, earthLookPoint.y, 1.0f);
  glm::vec4 sceneLookPoint = mWorldRotation * glm::transpose(r) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

  // Multiply by the world rotation matrix to find the axis
  //glm::vec4 sceneAxis = sceneLookPoint; //mWorldRotation * sceneLookPoint;
  //LOGI("scene look point: %f %f %f", sceneLookPoint.x, sceneLookPoint.y, sceneLookPoint.z);

  // Hack to do this once for touch events:
  if(mDoRotation){
    mWorldRotation = glm::rotate(mWorldRotation, 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    mDoRotation = false;
  }
/*
  m = m * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(sceneLookPoint.x, sceneLookPoint.y, sceneLookPoint.z),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  m = m * mWorldRotation; // Rotate the world
*/
  // Transform scene coordinates to image coordinates
  // TODO: probably clip nearer
  //m = glm::perspective(90.0f, aspectRatio, 0.5f, 100.0f) * mDeviceAxisMapping * r;
  m = glm::perspective(105.0f, aspectRatio, 0.5f, 100.0f) * r * mDeviceAxisMapping * mWorldRotation;
  //m = glm::perspective(90.0f, aspectRatio, 0.5f, 100.0f) * r;

  return(m);
}

/* Loads a scene from a file.  The path should be to an asset text file, not
 * a hard path in the filesystem.  Returns true on success.  */
bool Scene::load(const char* path)
{
  // If the graphics context doesn't exist yet, nothing here will work right.
  assert(mGraphicsConfigured);

    Model* theBox = new Model("/sdcard/rot/cube.obj");
    theBox->loadTexture("textures/burlwood.png");
    mDynamicObjects.push_back(theBox);

    /*
    Model* theBunny = new Model("/sdcard/rot/bunny.obj", attributeLocs);
    mMovingObjects.push_back(theBunny);
    */

    // Build a 10x10x10 cube
    Wall* front = new Wall(point3(-5.0f,  5.0f, -5.0f),
                           point3( 5.0f,  5.0f, -5.0f),
                           point3(-5.0f, -5.0f, -5.0f),
                           0.25f);
    front->loadTexture("textures/cube_front.png");
    mStaticObjects.push_back(front);

    Wall* back = new Wall(point3(-5.0f,  5.0f, 5.0f),
                          point3( 5.0f,  5.0f, 5.0f),
                          point3(-5.0f, -5.0f, 5.0f),
                          0.25f);
    back->loadTexture("textures/cube_back.png");
    mStaticObjects.push_back(back);

    Wall* left = new Wall(point3(-5.0f,  5.0f,  5.0f),
                          point3(-5.0f,  5.0f, -5.0f),
                          point3(-5.0f, -5.0f,  5.0f),
                          0.25f);
    left->loadTexture("textures/cube_left.png");
    mStaticObjects.push_back(left);

    Wall* right = new Wall(point3(5.0f,  5.0f,  5.0f),
                           point3(5.0f,  5.0f, -5.0f),
                           point3(5.0f, -5.0f,  5.0f),
                           0.25f);
    right->loadTexture("textures/cube_right.png");
    mStaticObjects.push_back(right);

    Wall* top = new Wall(point3(-5.0f, 5.0f,  5.0f),
                         point3( 5.0f, 5.0f,  5.0f),
                         point3(-5.0f, 5.0f, -5.0f),
                         0.25f);
    top->loadTexture("textures/mud.png");
    mStaticObjects.push_back(top);

    Wall* bottom = new Wall(point3(-5.0f, -5.0f,  5.0f),
                            point3( 5.0f, -5.0f,  5.0f),
                            point3(-5.0f, -5.0f, -5.0f),
                            0.25f);
    bottom->loadTexture("textures/burlwood.png");
    mStaticObjects.push_back(bottom);
}


/* Updates all of the positions, etc. */
void Scene::update()
{
  float dt = 0.06; // Time differential, in seconds TODO: calculate, don't hardcode

  // Move the character

  // Update particle system

  // Apply gravity/physics to movable objects and detect collisions
  glm::vec4 gravity = mWorldRotation * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
  LOGI("Gravity: %f  %f  %f", gravity.x, gravity.y, gravity.z);
  for(int i = 0; i < mDynamicObjects.size(); i++){
    mDynamicObjects[i]->applyGravity(gravity, mStaticObjects, dt);
  }


  // Check if the level is complete

  // Check if the player died
}

void Scene::renderFrame(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepthf(1.0f);
  checkGlError("glClearColor");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  checkGlError("glClear");

  glUseProgram(gProgram);
  checkGlError("glUseProgram");

  // Set up the camera projection matrix
  mProjectionMatrix = calculateCameraView(mCameraPosition, mAspectRatio);
  glUniformMatrix4fv(mContext.uniformProjection, 1, false, (GLfloat*)glm::value_ptr(mProjectionMatrix));

  // Set up the lighting
  glUniform4f(mUniformAmbient, 0.0f, 0.1f, 0.3f, 1.0f);
  glUniform4f(mUniformDiffuse, 0.8f, 0.0f, 0.0f, 1.0f);
  //glUniform4f(specularUniform, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(mContext.uniformLightPos, -3.0f, 3.0f, 0.0f, 1.0f);
  glUniform3fv(mContext.uniformCameraPos, 1, (GLfloat*)glm::value_ptr(mCameraPosition));
  checkGlError("set uniforms");

  // Draw the objects in the scene
  for(int i = 0; i < mDynamicObjects.size(); i++){
    glUniformMatrix4fv(mContext.uniformModelView, 1, false,
        (GLfloat*)glm::value_ptr(mDynamicObjects[i]->positionMatrix()));
    mDynamicObjects[i]->render(mContext);
  }
  checkGlError("Render dynamic objects");

  // Set the identity matrix and render all of the fixed objects
  glUniformMatrix4fv(mContext.uniformModelView, 1, false, IDENTITY_MATRIX_4);
  for(int i = 0; i < mStaticObjects.size(); i++){
    mStaticObjects[i]->render(mContext);
  }
  checkGlError("Render static objects");

}

void Scene::touchEvent(float x, float y)
{
  LOGI("Touch event: %f %f", x, y);
/*
  if(y > 500){
    mCameraPosition.z = mCameraPosition.z + 0.5f;
  }
  else{
    mCameraPosition.z = mCameraPosition.z - 0.5f;
  }
*/

  //mWorldRotation = glm::rotate(mWorldRotation, 45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
  mDoRotation = true;
}

