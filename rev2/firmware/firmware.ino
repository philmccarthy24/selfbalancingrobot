#include <Wire.h>
#include <elapsedMillis.h>
#include "EventBus.h"
#include "RotaryEncoder.h"
#include "UserInterface.h"
#include "AHRS.h"

EventBus eventBus;
UserInterface gui(&eventBus);
RotaryEncoder dial1(0, 34, 35, 26, &eventBus);
RotaryEncoder dial2(1, 38, 37, 27, &eventBus);
//RotaryEncoder dial3(2, 40, 39, 28, &eventBus);
AHRS ahrs(&eventBus);

elapsedMillis msTimer;
const int AHRS_PERIOD_MS = 1000 / FILTER_UPDATE_RATE_HZ;

void setup() {
  Serial.begin(115200);

  // setup message subscriptions
  eventBus.Subscribe(&ahrs);
  eventBus.Subscribe(&gui);

  if (!gui.Initialise() || 
    !ahrs.Initialise())
    for(;;) // Printed errors to serial already - so spin forever

  Wire.setClock(400000); // set I2C clock to 400KHz - ahrs seems to need this
}

void loop() {
  // poll physical controls - this could result in events being fired
  dial1.Poll();
  dial2.Poll();
  //dial3.Poll();

  // publish a timer message every 10ms, to kick off
  // AHRS and motion control
  if (msTimer >= AHRS_PERIOD_MS) {
    msTimer = msTimer - AHRS_PERIOD_MS;
    eventBus.Publish(0, EEventType::TimerTimeout, &AHRS_PERIOD_MS);
  }

}

/*
void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}
*/

/*
void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}
*/
