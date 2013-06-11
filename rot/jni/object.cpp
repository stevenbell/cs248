#include "jnibridge.h"
#include "gl_util.h"
#include "object.h"
#include <gtc/matrix_transform.hpp>

const GLfloat RenderContext::IDENTITY_MATRIX_4[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                                                      0.0f, 1.0f, 0.0f, 0.0f,
                                                      0.0f, 0.0f, 1.0f, 0.0f,
                                                      0.0f, 0.0f, 0.0f, 1.0f};
Object::Object() :
  mPosition(1.0f), mUseTexture(false)
{}

Object::~Object()
{
  if(mUseTexture){
    glDeleteTextures(1, &mTextureBuf);
  }
}

/*
 * Returns true if the texture loads and is valid, false otherwise.
 */
bool Object::loadTexture(const char* filename)
{
  mTextureImage = JniBridge::instance()->loadPng(filename);

  // Set up textures
  glGenTextures(1, &mTextureBuf);
  glBindTexture(GL_TEXTURE_2D, mTextureBuf);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

  checkGlError("setting texture params");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureImage.width, mTextureImage.height,
     0, GL_RGBA, GL_UNSIGNED_BYTE, mTextureImage.data);

  checkGlError("loadTexture - glTexImage2D");

  // TODO: error checking; return false if something fails
  mUseTexture = true;
  return mUseTexture;
}

void Object::applyGravity(const glm::vec3 gravity, const std::vector<Object*> &fixedObjects, const float dt)
{
  mVelocity += gravity * dt;

  glm::vec3 oldPosition = glm::vec3(mPosition[3][0], mPosition[3][1], mPosition[3][2]);
  glm::vec3 newPosition = oldPosition + dt * mVelocity;

  for(int i = 0; i < fixedObjects.size(); i++){
    if(fixedObjects[i]->collidesWith(newPosition, 1.0)){ // TODO: use the object bounds here
      // Make the normal component zero by projecting the velocity onto the
      // normal and subtracting that value.
      glm::vec3 cNorm = fixedObjects[i]->collisionNormal(newPosition);
      glm::vec3 normalForce = glm::dot(mVelocity, cNorm) * cNorm;
      mVelocity -= normalForce;
      //LOGI("nForce %d: %f %f %f", i, normalForce.x, normalForce.y, normalForce.z);
      // TODO: friction on the tangential component
      // charVelocity *= 1.0 - surfaceFriction; // 0.0 is ice, 1.0 is glue
    }
  }

  mPosition[3][0] = oldPosition.x + mVelocity.x;
  mPosition[3][1] = oldPosition.y + mVelocity.y;
  mPosition[3][2] = oldPosition.z + mVelocity.z;

  /*
  // TODO: replace all this junk with real collision detection
  mPosition[3][0] += mVelocity.x * -dt;
  if(mPosition[3][0] > 4.5){ mPosition[3][0] = 4.5; mVelocity[0] = 0; }
  if(mPosition[3][0] < -4.5){ mPosition[3][0] = -4.5; mVelocity[0] = 0; }
  mPosition[3][1] += mVelocity.y * dt;
  if(mPosition[3][1] > 4.5){ mPosition[3][1] = 4.5; mVelocity[1] = 0; }
  if(mPosition[3][1] < -4.5){ mPosition[3][1] = -4.5; mVelocity[1] = 0; }

  mPosition[3][2] = -3.0f; //+= mVelocity.z * dt;
  */
}

void Object::setPosition(float x, float y, float z)
{
  mPosition[3][0] = x;
  mPosition[3][1] = y;
  mPosition[3][2] = z;
}

/* Sets the rotation by rotating around each world? axis in succession. */
void Object::setRotation(float x, float y, float z)
{
  glm::mat4 newPosition(1.0);
  newPosition = glm::rotate(newPosition, x, glm::vec3(1.0f, 0.0f, 0.0f));
  newPosition = glm::rotate(newPosition, y, glm::vec3(0.0f, 1.0f, 0.0f));
  newPosition = glm::rotate(newPosition, z, glm::vec3(0.0f, 0.0f, 1.0f));
  newPosition[3][0] = mPosition[3][0]; // Copy over the translation
  newPosition[3][1] = mPosition[3][1];
  newPosition[3][2] = mPosition[3][2];
  mPosition = newPosition;
}

void Object::render(const RenderContext c)
{
  // Do any object-specific setup?
  // For now, we're doing the position matrix outside...
  subrender(c); // Call the child's rendering method
}

glm::vec3 Object::collisionNormal(glm::vec3 p)
{
  vec3 normal = collisionNormal(vec3(p.x, p.y, p.z));
  return(glm::vec3(normal.x, normal.y, normal.z));
}
