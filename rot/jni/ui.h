#ifndef __UI_H__
#define __UI_H__


class UiElement
{
public:
  UiElement(float x, float y, float w, float h, const char* imageName);
  void render(RenderContext c);
  bool pointIn(float x, float y);

  float xPos, yPos, width, height;

private:
  GLuint mVertexBuf;
  GLuint mTexCoordBuf;

  Image mArrowImage;
  GLuint mArrowTexture;
};

class Ui
{
public:
  Ui();
  void render(RenderContext c);
  void handleTouchEvent(float x, float y, int action);
  bool forward() { return mForwardDown; };
  bool backward() { return mBackDown; };

private:

  UiElement mForwardArrow;
  UiElement mBackArrow;

  bool mForwardDown;
  bool mBackDown;
  bool mJumpDown;
};

#endif
