
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
