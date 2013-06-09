
#include "log.h"
#include "orientation.h"

#define SENSOR_LOOPER_ID 20
// Somehow this got missed in the NDK
#define ASENSOR_TYPE_ROTATION_VECTOR 11


Orientation::Orientation()
{
  ALooper* looper = ALooper_forThread();
  if(looper == NULL){
    looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
  }

  mSensorManager = ASensorManager_getInstance();
  mOrientationSensor = ASensorManager_getDefaultSensor(mSensorManager, ASENSOR_TYPE_ROTATION_VECTOR);
  mSensorEventQueue = ASensorManager_createEventQueue(mSensorManager, looper, SENSOR_LOOPER_ID, NULL, NULL);

  ASensorEventQueue_enableSensor(mSensorEventQueue, mOrientationSensor);
  ASensorEventQueue_setEventRate(mSensorEventQueue, mOrientationSensor, 1000L * 100); // 100ms interval = 10 Hz (maybe)
}

Orientation::~Orientation()
{
  ASensorEventQueue_disableSensor(mSensorEventQueue, mOrientationSensor);
}

void Orientation::updateOrientation()
{
  ASensorEvent e;
  // If there are events, drain them and grab only the last one
  // If there aren't, then don't set anything
  if(ASensorEventQueue_hasEvents(mSensorEventQueue) > 0){
    while(ASensorEventQueue_hasEvents(mSensorEventQueue) > 0){
      ASensorEventQueue_getEvents(mSensorEventQueue, &e, 1);
      //LOGI("Sensor: %f %f %f %f", e.data[3], e.data[0], e.data[1], e.data[2]);
    }
    mQuat = Quat(e.data[3], e.data[0], e.data[1], e.data[2]);
    //LOGI("Done, setting.");
  }
}

glm::mat4 Orientation::getCameraOrientation()
{
  updateOrientation();
  return(mQuat.rotationMatrix());
}

const float Orientation::PROJECTION_THRESH = 0.8;

Orientation::CardinalRotation Orientation::getCardinalRotation()
{
  // Project the "up" and "right" axes of the device onto the "up" axis of the world,
  // and figure out which one is larger.  This is the +Y and +X device axes, and the
  // world +Z axis.
  // The desired projection is just third column of the matrix.
  // We also do a hysterisis and have a minimum to make sure that the device isn't pointing down.
  glm::mat4 r = getCameraOrientation();

  glm::vec4 upProjection = r * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
  float top = upProjection.y; // TODO: r[1][2];
  float right = upProjection.x; // TODO: r[0][2];

  int orientation;
  if(top > PROJECTION_THRESH){
    return(ROTATION_UP);
  }
  else if(top < -0.8){
    return(ROTATION_180);
  }
  else if(right > 0.8){
    return(ROTATION_90);
  }
  else if(right < -0.8){
    return(ROTATION_270);
  }

  return(ROTATION_UNKNOWN);
}
