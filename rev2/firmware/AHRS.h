#ifndef __AHRS_H_
#define __AHRS_H_

#include "EventBus.h"

#define FILTER_UPDATE_RATE_HZ 100

typedef struct _SOrientationPayload {
  float roll;
  float pitch;
  float yaw;
  float gyroX;
  float gyroY;
  float gyroZ;
} SOrientationPayload;

class Adafruit_Sensor;

class AHRS : public INotifiable
{
public:
  AHRS(EventBus* pEventBus);
  bool Initialise(void);

  virtual void OnEventNotify(int sourceId, EEventType type, const void* eventData) override;

private:
  Adafruit_Sensor* m_accelerometer;
  Adafruit_Sensor* m_gyroscope;
  Adafruit_Sensor* m_magnetometer;
  EventBus* m_pEventBus;
  SOrientationPayload m_lastUpdate;
};

#endif