
#include "object.h"
#include "gl_util.h"
#include "jnibridge.h"
#include "ui.h"

UiElement::UiElement(float x, float y, float w, float h, const char* imageName)
{
  xPos = x; yPos = y; // Save for later...
  width = w; height = h;

  glGenBuffers(1, &mVertexBuf);
  glGenBuffers(1, &mTexCoordBuf);
  glGenTextures(1, &mArrowTexture);

  float vertices[8] = {x, y, x + w, y, x, y + h, x + w, y + h};
  float texCoords[8] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, texCoords, GL_STATIC_DRAW);


  mArrowImage = JniBridge::instance()->loadPng(imageName);
  glBindTexture(GL_TEXTURE_2D, mArrowTexture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

  checkGlError("ui setting texture params");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mArrowImage.width, mArrowImage.height,
     0, GL_RGBA, GL_UNSIGNED_BYTE, mArrowImage.data);

  checkGlError("ui - glTexImage2D");
}

void UiElement::render(RenderContext c)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mArrowTexture);

  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuf);
  glVertexAttribPointer(c.attrVertexPosition, 2, GL_FLOAT, false, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuf);
  glVertexAttribPointer(c.attrTexCoord, 2, GL_FLOAT, false, 0, 0);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  checkGlError("Rendered UI");
}

/* Tests if a particular location is within the bounding box of this UI element */
bool UiElement::pointIn(float x, float y)
{
  return (x > xPos) && (x < xPos + width) && (y > yPos) && (y < yPos + height);
}

Ui::Ui() :
  mForwardArrow(0.75, 0.80, 0.25, 0.2, "textures/forwardarrow.png"),
  mBackArrow(0.75, 0.60, 0.25, 0.2, "textures/backarrow.png"),
  mForwardDown(false),
  mBackDown(false),
  mJumpDown(false)
{}


/* Copied from MotionEvent documentation: */
#define ACTION_DOWN 0x00
#define ACTION_UP 0x01
#define ACTION_MOVE 0x02

void Ui::handleTouchEvent(float x, float y, int action)
{
  if(action == ACTION_DOWN || action == ACTION_MOVE){
    mForwardDown = mForwardArrow.pointIn(x, y);
    mBackDown = mBackArrow.pointIn(x, y);
  }
  else if(action == ACTION_UP){
    mForwardDown = mForwardDown && !mForwardArrow.pointIn(x, y);
    mBackDown = mBackDown && !mBackArrow.pointIn(x, y);
  }
}

void Ui::render(RenderContext c)
{
  mForwardArrow.render(c);
  mBackArrow.render(c);
}

