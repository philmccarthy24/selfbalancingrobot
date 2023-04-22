#include <Wire.h>
#include <elapsedMillis.h>
#include "EventBus.h"
#include "RotaryEncoder.h"
#include "UserInterface.h"
#include "AHRS.h"
#include "MotionController.h"
#include <ArduinoLog.h>

EventBus eventBus;
UserInterface gui(&eventBus);
RotaryEncoder dial1(0, 34, 35, 26, &eventBus);
RotaryEncoder dial2(1, 38, 37, 27, &eventBus);
//RotaryEncoder dial3(2, 40, 39, 28, &eventBus);
AHRS ahrs(&eventBus);
MotionController motionController(&eventBus);

elapsedMillis msTimer;
const int AHRS_PERIOD_MS = 1000 / FILTER_UPDATE_RATE_HZ;

void setup() {
  Serial.begin(115200);
  while (!Serial) {} // wait for serial interace to connect

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.traceln(F("Setting I2C clock to 400kHz"));
  Wire.setClock(400000); // set I2C clock to 400KHz - ahrs seems to need this

  Log.traceln(F("Setting up event subscriptions..."));
  // setup message subscriptions
  eventBus.Subscribe(&ahrs);
  eventBus.Subscribe(&gui);
  eventBus.Subscribe(&motionController);

  if (!gui.Initialise() || !ahrs.Initialise() || !motionController.Initialise())
    for(;;) // Printed errors to serial already - so spin forever

  Log.traceln(F("Initialised"));
}

void loop() {
  // poll physical controls - this could result in events being fired
  dial1.Poll();
  dial2.Poll();
  //dial3.Poll();

  // publish a timer message every 10ms, to trigger
  // AHRS and then consequentially motion control
  if (msTimer >= AHRS_PERIOD_MS) {
    msTimer = msTimer - AHRS_PERIOD_MS;
    eventBus.Publish(0, EEventType::TimerTimeout, &AHRS_PERIOD_MS);
  }
}

