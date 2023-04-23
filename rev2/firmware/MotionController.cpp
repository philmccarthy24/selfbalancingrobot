#include "MotionController.h"
#include "AHRS.h"
#include <EEPROM.h>
#include <HardwareSerial.h>
#include <ODriveArduino.h>
#include <Wire.h>
#include <Arduino.h>
#include <ArduinoLog.h>

#define MOTION_CONTROLLER_SETTINGS_EEPROM_ADDRESS 140

// Printing with stream operator helper functions
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

// Teensy 4.1 - Serial1
// pin 0: RX - connect to ODrive TX (GPIO pin 1 on ODrive)
// pin 1: TX - connect to ODrive RX (GPIO pin 2 on ODrive)
HardwareSerial& odrive_serial = Serial1;
ODriveArduino odrive(odrive_serial);

MotionController::MotionController(EventBus* pEventBus) : 
  m_pEventBus(pEventBus)
{
}

bool MotionController::Initialise()
{
  Log.traceln(F("MotionController::Initialise"));

  //read eeprom at offset 140 to get persisted Kp,Ki,Kd constants
  float Kpid[3];
  EEPROM.get(MOTION_CONTROLLER_SETTINGS_EEPROM_ADDRESS,Kpid);
  if (isnan(Kpid[0]) || isnan(Kpid[1]) || isnan(Kpid[2]))
  {
    Log.traceln(F("PID constants not set, initialising to 0.0..."));
    Kpid[0] = 0.0f;
    Kpid[1] = 0.0f;
    Kpid[2] = 0.0f;
    EEPROM.put(MOTION_CONTROLLER_SETTINGS_EEPROM_ADDRESS,Kpid);
    Log.traceln(F("PID constants initialised."));
  }
  m_Kp = Kpid[0];
  m_Ki = Kpid[1];
  m_Kd = Kpid[2];

  Log.traceln(F("Read PID vals from EEPROM: Kp=%F, Kd=%F"), m_Kp, m_Kd);

  // publish a message with read settings, to synchronise interested listeners
  m_pEventBus->Publish(0, EEventType::SettingsSync, Kpid);

  // ODrive uses 115200 baud
  odrive_serial.begin(115200);

  Log.traceln(F("Setting ODrive current limits to 10A per wheel"));
  //odrive_serial << "w axis0.motor.config.current_lim " << 5.0f << '\n';
  //odrive_serial << "w axis1.motor.config.current_lim " << 5.0f << '\n';

  odrive_serial << "w axis0.controller.config.enable_torque_mode_vel_limit false\n";
  odrive_serial << "w axis1.controller.config.enable_torque_mode_vel_limit false\n";

  int requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL;
  Log.traceln(F("Engaging motors"));
  odrive.run_state(0, requested_state, false);
  odrive.run_state(1, requested_state, false);

  return true;
}

void MotionController::OnEventNotify(int sourceId, EEventType type, const void* eventData)
{
  // AHRS updates tick on a 100hz timer, which we will match to control wheel torque.
  // Note we have the flexibility to change this by creating an additional independent timer, caching AHRS
  // update messages and then doing PID processing and motion control on the independent timer tick.

  if (type == EEventType::AHRSUpdate) {
    SOrientationPayload* pOri = (SOrientationPayload*)eventData;

    // TODO: emergency cutoffs constants as config params rather than hard coded
    float balanceCurrent = 0.0f;
    if (pOri->roll < HALF_PI / 2) // 45 degrees
    {
      balanceCurrent = (m_Kp * pOri->roll) + (m_Kd * pOri->gyroX); //m_Ki integral term redundant for now
    }
    if (balanceCurrent > 10.0f)
      balanceCurrent = 10.0f; // manual current limiter
    
    // log every half second
    if (logTimer >= 500) {
      logTimer = logTimer - 500;
      Log.traceln(F("balanceCurrent set to %F"), balanceCurrent);
    }

    odrive.SetCurrent(0, (float)MOTOR_DIR_M0 * balanceCurrent);
    odrive.SetCurrent(1, (float)MOTOR_DIR_M1 * balanceCurrent);

    // TODO: control via RC
  } else if (type == EEventType::SettingsSync && sourceId == 1) {
    // (cached) PID vals have been updated in UI. Update master ones in this object
    const float* pidArray = (const float*)eventData;
    m_Kp = pidArray[0];
    m_Ki = pidArray[1];
    m_Kd = pidArray[2];
  } else if (type == EEventType::StoreData)
  {
    // persist new pid settings to eeprom
    float Kpid[3];
    Kpid[0] = m_Kp;
    Kpid[1] = m_Ki;
    Kpid[2] = m_Kd;
    EEPROM.put(MOTION_CONTROLLER_SETTINGS_EEPROM_ADDRESS,Kpid);
  }
}

/*
// standby function
void MotionController::Control() 
{
  Serial.println("Setting current on motors");
  
  delay(5000);
  int requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL;
  Serial.println("Engaging motors");
  odrive.run_state(0, requested_state, false);
  odrive.run_state(1, requested_state, false);
  delay(2000);
  Serial.println("Disengaging motors");
  requested_state = AXIS_STATE_IDLE;
  odrive.run_state(0, requested_state, false);
  odrive.run_state(1, requested_state, false);
  delay(2000);
}
*/