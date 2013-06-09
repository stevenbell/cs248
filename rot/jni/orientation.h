#ifndef __ORIENTATION_H__
#define __ORIENTATION_H__
/* */

#include <android/sensor.h>
#include "quaternion.h"

class Orientation
{
public:
  enum CardinalRotation{
    ROTATION_UNKNOWN,
    ROTATION_UP,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270
  };

  Orientation();
  ~Orientation();
  glm::mat4 getCameraOrientation();
  CardinalRotation getCardinalRotation();

private:
  static const float PROJECTION_THRESH;
  void updateOrientation();

  ASensorManager* mSensorManager;
  const ASensor* mOrientationSensor;
  ASensorEventQueue* mSensorEventQueue;

  Quat mQuat;
};

#endif
