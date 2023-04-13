#ifndef __MOTIONCONTROLLER_H_
#define __MOTIONCONTROLLER_H_

#define MOTOR_DIR_M0 -1
#define MOTOR_DIR_M1 1

#include <HardwareSerial.h>
#include <ODriveArduino.h>

class MotionController 
{
public:
  MotionController();
  void Setup();

private:
  void Control();

  // ODrive object
  ODriveArduino odrive;
  
};

#endif