#include <android/log.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "model.h"
#include "wall.h"

#include "gl_util.h"
#include "jnibridge.h"
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
  : mGraphicsConfigured(false), mPendingScene(NULL),
    mXMin(-10.0), mXMax(10.0), mYMin(-10.0), mYMax(10.0), mZMin(-10.0), mZMax(10.0),
    mGameStatus(PLAYING),
    mFinishMarker(NULL),
    mUi(NULL),
    mWidth(480), mHeight(640), mAspectRatio(1.0f)
{
    reset();
}

void Scene::reset(void)
{
    mWorldRotation = glm::mat4(1.0f); // Identity matrix

    mDeviceAxisMapping = glm::mat4( 1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, -1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f);

    mCameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    mCharVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
    mLightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    mActiveLights = 0;

    // Empty the lists of objects and such...
    while(!mDynamicObjects.empty()){
      Object* o = mDynamicObjects.back();
      delete o;
      mDynamicObjects.pop_back();
    }
    while(!mStaticObjects.empty()){
      Object* o = mStaticObjects.back();
      delete o;
      mStaticObjects.pop_back();
    }
}

bool Scene::setupGraphics(int w, int h) {
    // Print debugging information
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    // Compile and link the shader program
    gProgram = createProgram("shaders/world.vert", "shaders/lighttexture.frag");
    uiShaderProgram = createProgram("shaders/tex_passthrough.vert", "shaders/dumbtexture.frag");
    if (!gProgram || !uiShaderProgram) {
        LOGE("Failed to compile at least one shader program!");
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


    // Handles for UI rendering shader
    mUiContext.attrVertexPosition = glGetAttribLocation(uiShaderProgram, "vertexPosition");
    mUiContext.attrTexCoord = glGetAttribLocation(uiShaderProgram, "vertexTexCoord");

    checkGlError("get uniform/attributes locations");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");

    mWidth = w;
    mHeight = h;
    mAspectRatio = (float)w / h;

    mUi = new Ui;

    mGraphicsConfigured = true;

    // If we've already been asked to load a scene (but couldn't, because there
    // wasn't a graphics context), then load it now.
    if(mPendingScene != NULL){
      load(mPendingScene);
    }

    return true;
}


void Scene::calculateDeviceRotation()
{
  // Calculate the primary orthogonal direction by taking the scene look axis
  // and finding the largest component

  // We convert a scene point into GL coordinates using
  //   projectionMatrix * r * mDeviceAxisMapping * mWorldRotation * m;
  // So we can convert a look-axis in GL coordinates (the Z axis) into scene
  // coordinates by taking the inverse (aka, transpose):
  //   glm::transpose(r * mDeviceAxisMapping * mWorldRotation)
  glm::mat4 r = mOrientation.getCameraOrientation();
  glm::vec4 sceneLook = glm::transpose(r * mDeviceAxisMapping * mWorldRotation) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

  glm::vec3 rotationAxis;
  if(fabs(sceneLook.x) > fabs(sceneLook.y) && fabs(sceneLook.x) > fabs(sceneLook.z)){
    rotationAxis = glm::vec3(copysignf(1.0f, -sceneLook.x), 0.0f, 0.0f);
  }
  else if(fabs(sceneLook.y) > fabs(sceneLook.z)){ // We know y or z is > x; which one?
    rotationAxis = glm::vec3(0.0f, copysignf(1.0f, -sceneLook.y), 0.0f);
  }
  else{
    rotationAxis = glm::vec3(0.0f, 0.0f, copysignf(1.0f, -sceneLook.z));
  }

  //LOGI("Look: %f  %f  %f", sceneLook.x, sceneLook.y, sceneLook.z);
  //LOGI("rotation axis: %f  %f  %f", rotationAxis.x, rotationAxis.y, rotationAxis.z);

  static Orientation::CardinalRotation prevRotation = Orientation::ROTATION_UNKNOWN;
  Orientation::CardinalRotation cr = mOrientation.getCardinalRotation();
  if((prevRotation == Orientation::ROTATION_UP && cr == Orientation::ROTATION_90) ||
     (prevRotation == Orientation::ROTATION_90 && cr == Orientation::ROTATION_180) ||
     (prevRotation == Orientation::ROTATION_180 && cr == Orientation::ROTATION_270) ||
     (prevRotation == Orientation::ROTATION_270 && cr == Orientation::ROTATION_UP)){
     mWorldRotation = glm::rotate(mWorldRotation, -90.0f, rotationAxis);
  LOGI("rotate -90: %f  %f  %f", rotationAxis.x, rotationAxis.y, rotationAxis.z);
  }
  else if((prevRotation == Orientation::ROTATION_UP && cr == Orientation::ROTATION_270) ||
          (prevRotation == Orientation::ROTATION_90 && cr == Orientation::ROTATION_UP) ||
          (prevRotation == Orientation::ROTATION_180 && cr == Orientation::ROTATION_90) ||
          (prevRotation == Orientation::ROTATION_270 && cr == Orientation::ROTATION_180)){
    mWorldRotation = glm::rotate(mWorldRotation, 90.0f, rotationAxis);
  LOGI("rotate +90: %f  %f  %f", rotationAxis.x, rotationAxis.y, rotationAxis.z);
  }

  if(cr != Orientation::ROTATION_UNKNOWN){
    prevRotation = cr;
  }
}

glm::mat4 Scene::calculateCameraView(glm::vec3 cameraPosition, float aspectRatio)
{
  glm::mat4 m(1.0); // Identity matrix

  // Translate and then rotate the world relative to the camera
  m = glm::translate(m, -mCameraPosition);

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

/*
  m = m * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(sceneLookPoint.x, sceneLookPoint.y, sceneLookPoint.z),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  m = m * mWorldRotation; // Rotate the world
*/
  // Transform scene coordinates to image coordinates
  // TODO: probably clip nearer
  //m = glm::perspective(90.0f, aspectRatio, 0.5f, 100.0f) * mDeviceAxisMapping * r;
  m = glm::perspective(105.0f, aspectRatio, 0.5f, 100.0f) * r * mDeviceAxisMapping * mWorldRotation * m;

  return(m);
}

/* Loads a scene from a file.  The path should be to an asset text file, not
 * a hard path in the filesystem.  Returns true on success.  */
bool Scene::load(const char* path)
{
  // If the graphics context doesn't exist yet, nothing here will work right.
  // Save the path that was requested, and when we get the graphics context,
  // call this again.  If the path is the saved path, don't copy it; that ends badly.
  if(path != mPendingScene){
    if(mPendingScene != NULL){
      // If there was a previous request, delete it
      delete mPendingScene;
    }
    mPendingScene = (char*)malloc(strlen(path) + 1); // +1 for null terminator
    strcpy(mPendingScene, path);
  }

  if(!mGraphicsConfigured){
    return false;
  }

  // Load the level configuration file
  char* text;
  JniBridge::instance()->loadText(path, &text);

  // The level format consists of a series of
  // KEY : DATA \n
  // so we can split it with ":" and "\n" and parse the pieces.
  char* key;
  char* value;
  key = strtok(text, ":\n"); // Could use just ":", but this lets us eat empty newlines
  while(key != NULL){
    if(key[0] == '#'){ // Eat the comment and move on to the next line
      LOGI("Skipping comment %s", key);
      key = strtok(NULL, ":\n");
      continue;
    }

    value = strtok(NULL, "\n");

    LOGI("Key: %s  Value: %s", key, value);

    // Walls
    if(strcmp(key, "wall") == 0){
      float x1, y1, z1, x2, y2, z2, x3, y3, z3, width;
      char texPath[200];
      sscanf(value, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %s",
          &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &width, texPath);

      Wall* w = new Wall(point3(x1, y1, z1),
                         point3(x2, y2, z2),
                         point3(x3, y3, z3),
                         width);
      w->loadTexture(texPath);
      mStaticObjects.push_back(w);
    }
    else if(strcmp(key, "model") == 0){
      float x, y, z, scale;
      char path[200];
      sscanf(value, "%f, %f, %f, %f, %s", &x, &y, &z, &scale, path);

      Object* o = new Model("/sdcard/rot/cube.obj");
      o->loadTexture("textures/burlwood.png");
      o->setPosition(x, y, z);
      mDynamicObjects.push_back(o);
    }
    // Start position
    else if(strcmp(key, "start") == 0){
      float x, y, z;
      sscanf(value, "%f, %f, %f", &x, &y, &z);
      mCameraPosition = glm::vec3(x, y, z);
    }
    // Goal position
    else if(strcmp(key, "finish") == 0){
      float x, y, z;
      sscanf(value, "%f, %f, %f", &x, &y, &z);

      mFinishMarker = new Model("/sdcard/rot/star.obj");
      mFinishMarker->loadTexture("textures/yellow.png");
      mFinishMarker->setPosition(x, y, z);
      mFinishMarker->setRotation(90.0, 0.0, 0.0); // Upright wrt starting position
      mStaticObjects.push_back(mFinishMarker);
    }
    // Boundaries
    else if(strcmp(key, "bounds") == 0){
      sscanf(value, "%f, %f, %f, %f, %f, %f", &mXMin, &mXMax, &mYMin, &mYMax, &mZMin, &mZMax);
    }
    else if(strcmp(key, "light") == 0){
      // Note that all lighting is calculated in an un-rotated coordinate frame
      float x, y, z;
      sscanf(value, "%f, %f, %f", &x, &y, &z);

      //mLightPositions[mActiveLights]
      mLightPosition = glm::vec3(x, y, z);
    }

    key = strtok(NULL, ":\n");
  }

  free(text);

  mGameStatus = PLAYING; // Start a new game
}


/* Updates all of the positions, etc.
 * Returns a value indicating the state of the game. */
int Scene::update()
{
  float dt = 0.06; // Time differential, in seconds TODO: calculate, don't hardcode

  calculateDeviceRotation();

  glm::vec4 gravity = glm::transpose(mWorldRotation) * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
  glm::vec3 gravity3 = glm::vec3(gravity.x, gravity.y, gravity.z);
  //LOGI("Gravity: %f  %f  %f", gravity.x, gravity.y, gravity.z);

  // The running velocity does not integrate the way gravity does, so we can't just add it
  // The solution is to keep two velocity vectors, and check them together for collisions
  glm::vec3 fallVelocity = mCharVelocity;
  glm::vec3 runVelocity(0.0f, 0.0f, 0.0f);

  // Apply gravity to the character
  fallVelocity += gravity3;

  // Move the character
  if(mUi->forward() || mUi->backward()){
    // These lines are based on the camera view calculation; see that code for a full description
    glm::mat4 r = mOrientation.getCameraOrientation();
    glm::vec4 sceneLookPoint = glm::transpose(r * mDeviceAxisMapping * mWorldRotation) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

    glm::vec4 up = mWorldRotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 moveVector = sceneLookPoint - (up * glm::dot(sceneLookPoint, up));
    glm::vec3 move3 = glm::vec3(moveVector.x, moveVector.y, moveVector.z);

    // Increase and clamp the velocity
    mRunningSpeed += mUi->forward() ? -0.2 : 0.2;
    if(mRunningSpeed > 5.0f)  mRunningSpeed = 5.0f;
    if(mRunningSpeed < -5.0f)  mRunningSpeed = -5.0f;

    //LOGI("Scene look: %f %f %f", moveVector.x, moveVector.y, moveVector.z);
    // If the present velocity in the direction of running is less than the maximum,
    runVelocity = mRunningSpeed * glm::vec3(moveVector.x, moveVector.y, moveVector.z);
  }
  else{
    // If neither button is being pressed, stop
    mRunningSpeed = 0;
  }

  glm::vec3 newPosition = mCameraPosition + dt * (fallVelocity + runVelocity);

  for(int i = 0; i < mStaticObjects.size(); i++){
    if(mStaticObjects[i]->collidesWith(newPosition, 1.0)){
      // Make the normal component zero by projecting the velocity onto the
      // normal and subtracting that value.
      glm::vec3 cNorm = mStaticObjects[i]->collisionNormal(newPosition);
      float gNormalMag = glm::dot(fallVelocity, cNorm); // Magnitude of the normal force
      float rNormalMag = glm::dot(runVelocity, cNorm);
      // If the velocity is pointed into the normal (i.e, they have opposite signs),
      // then apply the force.  Otherwise, it's not a real collision.
      if(gNormalMag < 0){
        // Subtracting here, normalForce is the right direction and cNorm is negative
        fallVelocity -= gNormalMag * cNorm;
        // TODO: friction on tangential component
        // fallVelocity *= 1.0 - surfaceFriction; // 0.0 is ice, 1.0 is glue
      }
      if(rNormalMag < 0){
        runVelocity -= rNormalMag * cNorm;
      }
    }
  }

  // Now do the same for dynamic objects (e.g, crates)
  // First, recalculate the new position using the collisions we've already handled
  newPosition = mCameraPosition + dt * (fallVelocity + runVelocity);

  for(int i = 0; i < mDynamicObjects.size(); i++){
    if(mDynamicObjects[i]->collidesWith(newPosition, 1.0)){
      glm::vec3 cNorm = mDynamicObjects[i]->collisionNormal(newPosition);
      float fNormalMag = glm::dot(fallVelocity, cNorm); // Magnitude of the normal force
      float rNormalMag = glm::dot(runVelocity, cNorm); // Magnitude of the normal force
      // If the velocity is pointed into the normal (i.e, they have opposite signs),
      // then apply the force.  Otherwise, it's not a real collision.
      if(fNormalMag < 0){
        // Subtracting here, normalForce is the right direction and cNorm is negative
        //fallVelocity -= normalMag * cNorm;
        fallVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
      }
      if(rNormalMag < 0){
        runVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
      }
    }
  }

  //LOGI("Vel: %f %f %f", fallVelocity.x, fallVelocity.y, fallVelocity.z);

  // Now that we've handled collisions with walls, set the new position
  mCharVelocity = fallVelocity;
  mCameraPosition += dt * (fallVelocity + runVelocity);

  // Apply gravity/physics to movable objects and detect collisions



  for(int i = 0; i < mDynamicObjects.size(); i++){
    mDynamicObjects[i]->applyGravity(gravity3, mStaticObjects, dt);
  }

  // Check if the level is complete
  if(mFinishMarker->collidesWith(mCameraPosition, 2.0)){
    mGameStatus = WON;
  }

  // Check if the player died
  if(mCameraPosition.x < mXMin || mCameraPosition.x > mXMax ||
     mCameraPosition.y < mYMin || mCameraPosition.y > mYMax ||
     mCameraPosition.z < mZMin || mCameraPosition.z > mZMax){
    // Game over!
    //LOGI("Game over; player fell to death: %f %f %f", mCameraPosition.x, mCameraPosition.y, mCameraPosition.z);
    mGameStatus = LOST;
  }

  return(mGameStatus);
}

void Scene::renderFrame(void)
{
  // TODO: replace with real game over
  if(mGameStatus == LOST){
    glClearColor(0.5f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
  else if(mGameStatus == WON){
    glClearColor(0.0f, 0.5f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepthf(1.0f);
  checkGlError("glClearColor");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  checkGlError("glClear");

  // Set the program for 3D rendering and enable the necessary vertex arrays
  glUseProgram(gProgram);
  glEnableVertexAttribArray(mContext.attrVertexPosition);
  glEnableVertexAttribArray(mContext.attrVertexNormal);
  glEnableVertexAttribArray(mContext.attrTexCoord);
  checkGlError("glUseProgram");

  // Set up the camera projection matrix
  mProjectionMatrix = calculateCameraView(mCameraPosition, mAspectRatio);
  glUniformMatrix4fv(mContext.uniformProjection, 1, false, (GLfloat*)glm::value_ptr(mProjectionMatrix));

  // Set up the lighting
  //LOGI("Light position: %f %f %f", mLightPosition.x, mLightPosition.y, mLightPosition.z);

  glUniform4f(mUniformAmbient, 0.3f, 0.3f, 0.35f, 1.0f);
  glUniform4f(mUniformDiffuse, 0.8f, 0.8f, 0.6f, 1.0f);
  //glUniform4f(specularUniform, 1.0f, 1.0f, 1.0f, 1.0f);
  //glUniform3fv(mContext.uniformLightPos, 1, (GLfloat*)glm::value_ptr(mLightPosition));
  glUniform3f(mContext.uniformLightPos, mLightPosition.x, mLightPosition.y, mLightPosition.z);

  //glUniform3fv(mContext.uniformCameraPos, 1, (GLfloat*)glm::value_ptr(mCameraPosition));
  checkGlError("set uniforms");

  // Draw the objects in the scene
  for(int i = 0; i < mDynamicObjects.size(); i++){
    glUniformMatrix4fv(mContext.uniformModelView, 1, false,
        (GLfloat*)glm::value_ptr(mDynamicObjects[i]->positionMatrix()));
    mDynamicObjects[i]->render(mContext);
  }
  checkGlError("Render dynamic objects");

  // Draw the static objects in the scene
  for(int i = 0; i < mStaticObjects.size(); i++){
    glUniformMatrix4fv(mContext.uniformModelView, 1, false,
        (GLfloat*)glm::value_ptr(mStaticObjects[i]->positionMatrix()));
    mStaticObjects[i]->render(mContext);
  }
  checkGlError("Render static objects");

  // Draw the character
  glUniformMatrix4fv(mContext.uniformModelView, 1, false, RenderContext::IDENTITY_MATRIX_4);

  glUseProgram(uiShaderProgram);
  glEnableVertexAttribArray(mUiContext.attrVertexPosition);
  glEnableVertexAttribArray(mUiContext.attrTexCoord);

  glClear(GL_DEPTH_BUFFER_BIT); // Render the UI on top of everything else
  mUi->render(mUiContext);

  checkGlError("Rendering UI/end of renderFrame()");
}

void Scene::touchEvent(float x, float y, int action)
{
  //LOGI("Touch event: %f %f", x, y);

  //if(y < 500){
    // Convert point to GL screen coordinates where the UI lives
    // (0, 0) is the top left in touch coordinate space
    mUi->handleTouchEvent(x*2.0/mWidth - 1.0, 1.0 - y*2.0/mHeight, action);
  //LOGI("Touch event, GL space: %f %f", x*2.0/mWidth - 1.0f, 1.0f - y*2.0/mHeight);

    /*
  }
  else{
    if(action == 0x00){
      if(x < mWidth / 2){
        mWorldRotation = glm::rotate(mWorldRotation, -90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
      }
      else{
        mWorldRotation = glm::rotate(mWorldRotation, 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
      }
    }
  }
  */
}

