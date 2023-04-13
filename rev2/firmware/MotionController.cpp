#include "MotionController.h"

// Printing with stream operator helper functions
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

// Teensy 4.1 - Serial1
// pin 0: RX - connect to ODrive TX (GPIO pin 1 on ODrive)
// pin 1: TX - connect to ODrive RX (GPIO pin 2 on ODrive)
HardwareSerial& odrive_serial = Serial1;

MotionController::MotionController() : 
  odrive(odrive_serial)
{

}

void MotionController::Setup()
{
  // ODrive uses 115200 baud
  odrive_serial.begin(115200);

  Serial.println(F("Setting ODrive current limits to 5A per wheel"));
  odrive_serial << "w axis0.motor.config.current_lim " << 5.0f << '\n';
  odrive_serial << "w axis1.motor.config.current_lim " << 5.0f << '\n';
}

// standby function
void MotionController::Control() 
{
  Serial.println("Setting current on motors");
  odrive.SetCurrent(0, MOTOR_DIR_M0 * 0.5);
  odrive.SetCurrent(1, MOTOR_DIR_M1 * 0.5);
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