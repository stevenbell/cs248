#ifndef __ORIENTATION_H__
#define __ORIENTATION_H__
/* */

#include <android/sensor.h>
#include "quaternion.h"

class Orientation
{
public:
  Orientation();
  ~Orientation();
  glm::mat4 getCameraOrientation();

private:
  void updateOrientation();

  ASensorManager* mSensorManager;
  const ASensor* mOrientationSensor;
  ASensorEventQueue* mSensorEventQueue;

  Quat mQuat;
};

#endif
