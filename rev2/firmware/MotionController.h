#ifndef __MOTIONCONTROLLER_H_
#define __MOTIONCONTROLLER_H_

#include "EventBus.h"

#define MOTOR_DIR_M0 -1
#define MOTOR_DIR_M1 1

#include <HardwareSerial.h>
#include <ODriveArduino.h>

class MotionController : public INotifiable
{
public:
  MotionController(EventBus* pEventBus);
  virtual void OnEventNotify(int sourceId, EEventType type, const void* eventData) override;
  bool Initialise();

private:

  // ODrive object
  ODriveArduino odrive;
  
  EventBus* m_pEventBus;

  // PID constants
  float m_Kp;
  float m_Ki;
  float m_Kd;
  // angle cut off?
  // power limit/cut off?
};

#endif