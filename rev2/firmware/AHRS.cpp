// Full orientation sensing using NXP and a FXAS/FXOS 9-DoF sensor.
// You *must* perform a magnetic calibration before this code will work.
//
// Based on  https://github.com/PaulStoffregen/NXPMotionSense with adjustments
// to Adafruit Unified Sensor interface
// Cribbed from Adafruit example, full credit to Adafruit industries for their brilliant example code
// and libraries. Buy their original breakout modules to support them folks

#include "AHRS.h"
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_FXOS8700.h>
#include "EventBus.h"

Adafruit_FXOS8700 fxos = Adafruit_FXOS8700(0x8700A, 0x8700B);
Adafruit_FXAS21002C fxas = Adafruit_FXAS21002C(0x0021002C);

Adafruit_NXPSensorFusion filter;
Adafruit_Sensor_Calibration_EEPROM cal;

AHRS::AHRS(EventBus* pEventBus)
{
  m_pEventBus = pEventBus;
  m_lastUpdate = {};
}

bool AHRS::Initialise(void) 
{
  bool success = true;
  if (!cal.begin()) {
    Serial.println("Failed to initialize calibration helper");
    success = false;
  } else if (! cal.loadCalibration()) {
    Serial.println("No calibration loaded/found");
    success = false;
  }

  if (!fxos.begin() || !fxas.begin()) {
    Serial.println("Failed to find sensors");
    success = false;
  }
  m_accelerometer = fxos.getAccelerometerSensor();
  m_gyroscope = &fxas;
  m_magnetometer = fxos.getMagnetometerSensor();

  filter.begin(FILTER_UPDATE_RATE_HZ);

  return success;
}

void AHRS::OnEventNotify(int sourceId, EEventType type, const void* eventData) 
{
  if (type == EEventType::TimerTimeout && sourceId == 0) {
    // 100hz timer has ticked
    float gx, gy, gz;

    // Read the motion sensors
    sensors_event_t accel, gyro, mag;
    m_accelerometer->getEvent(&accel);
    m_gyroscope->getEvent(&gyro);
    m_magnetometer->getEvent(&mag);
  #if defined(AHRS_DEBUG_OUTPUT)
    Serial.print("I2C took "); Serial.print(millis()-timestamp); Serial.println(" ms");
  #endif

    cal.calibrate(mag);
    cal.calibrate(accel);
    cal.calibrate(gyro);
    // Gyroscope needs to be converted from Rad/s to Degree/s
    // the rest are not unit-important
    gx = gyro.gyro.x * SENSORS_RADS_TO_DPS;
    gy = gyro.gyro.y * SENSORS_RADS_TO_DPS;
    gz = gyro.gyro.z * SENSORS_RADS_TO_DPS;

    // Update the SensorFusion filter
    filter.update(gx, gy, gz, 
                  accel.acceleration.x, accel.acceleration.y, accel.acceleration.z, 
                  mag.magnetic.x, mag.magnetic.y, mag.magnetic.z);
  #if defined(AHRS_DEBUG_OUTPUT)
    Serial.print("Update took "); Serial.print(millis()-timestamp); Serial.println(" ms");
  #endif

    m_lastUpdate.roll = filter.getRoll();
    m_lastUpdate.pitch = filter.getPitch();
    m_lastUpdate.yaw = filter.getYaw();
    m_lastUpdate.gyroX = gyro.gyro.x;
    m_lastUpdate.gyroY = gyro.gyro.y;
    m_lastUpdate.gyroZ = gyro.gyro.z;

    m_pEventBus->Publish(0, EEventType::AHRSUpdate, &m_lastUpdate);
    
  #if defined(AHRS_DEBUG_OUTPUT)
    Serial.print("Took "); Serial.print(millis()-timestamp); Serial.println(" ms");
  #endif
  }
}

