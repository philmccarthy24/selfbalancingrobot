#include "UserInterface.h"
#include "AHRS.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // Monochrome OLED lib based on SSD1306 drivers
#include <Arduino.h>
#include <ArduinoLog.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

elapsedMillis ahrsScreenRefresh;

UserInterface::UserInterface(EventBus* pEventBus) :
  m_uiState(EUIState::Info),
  m_cachedKp(0.0f),
  m_cachedKd(0.0f),
  m_pEventBus(pEventBus)
{
}

bool UserInterface::Initialise() {
  Log.traceln(F("UserInterface::Initialise"));
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    return false;
  }

  display.clearDisplay();
  display.display();

  //display.setCursor(0, 0);     // Start at top-left corner
  //display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setTextSize(2);      // x2 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.println(F("SBR Firmware v0.1"));
  display.display();
  return true;
}

// handle controls using a state machine
// MVP: very simple operation. System can be put into
// AHRS monitor mode, or pid update mode.
// first RE button switches state.
void UserInterface::OnEventNotify(int sourceId, EEventType type, const void* eventData)
{
  if (type == EEventType::SettingsSync && sourceId == 0) {
    // this is a setting synchronisation message - settings just
    // read from eeprom persistant store
    const float* pPidSettingsArray = (const float*)eventData;
    m_cachedKp = pPidSettingsArray[0];
    m_cachedKd = pPidSettingsArray[2];
    if (m_uiState == EUIState::AdjustPID)
      DrawPIDScreen(); // update pid screen if displayed
    return; // return, as message handled.
  }

  switch (m_uiState) {
    case EUIState::Info :
      // don't render AHRS every 10ms, update every 200ms so we keep
      // timings accurate and not crash filter
      if (type == EEventType::AHRSUpdate && ahrsScreenRefresh >= 200) {
        ahrsScreenRefresh = 0;
        SOrientationPayload* pOri = (SOrientationPayload*)eventData;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(2);      // x2 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.println(F("INFO"));
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.setTextSize(1);      // 1:1 pixel scale
        display.print(F("Roll="));display.println(pOri->roll * RAD_TO_DEG,2);
        display.print(F("Pitch="));display.println(pOri->pitch * RAD_TO_DEG,2);
        display.print(F("Yaw="));display.println(pOri->yaw * RAD_TO_DEG,2);
        display.print(F("gyro X="));display.println(pOri->gyroX,2);
        display.print(F("gyro Y="));display.println(pOri->gyroY,2);
        display.print(F("gyro Z="));display.println(pOri->gyroZ,2);
        display.display();
      } else if (type == EEventType::RotaryEncoderButton) {
        if (sourceId == 0) { // first button will change screens
          m_uiState = EUIState::AdjustPID;
          DrawPIDScreen();
        } else if (sourceId == 1) { // second button will zero tilt
          m_pEventBus->Publish(0, EEventType::ZeroTilt, nullptr);
        }
      }
      break;
    case EUIState::AdjustPID :
      if (type == EEventType::RotaryEncoderPosition)
      {
        float delta = *((float*)eventData);
        if (delta != 0.0f) {
        if (sourceId == 0) // dial 1
          m_cachedKp += delta;
        else if (sourceId == 1) // dial 2
          m_cachedKd += delta;
        }
        // update MotorController PID values
        float cachedPids[3] = {};
        cachedPids[0] = m_cachedKp;
        cachedPids[1] = 0.0f; // not used yet
        cachedPids[2] = m_cachedKd;
        m_pEventBus->Publish(1, EEventType::SettingsSync, cachedPids);
        
        DrawPIDScreen();
      } 
      else if (type == EEventType::RotaryEncoderButton)
      {
        if (sourceId == 0) { // first button will change screen back to AHRS
          m_uiState = EUIState::Info;
          // timer tick will refresh screen, so don't need to do anything here
        } else if (sourceId == 1) { // second button will store new pid values
          m_pEventBus->Publish(0, EEventType::StoreData, nullptr);
        }
      }
  };
}

void UserInterface::DrawPIDScreen()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);      // x2 pixel scale
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(F("PID ADJUST"));
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.print(F("Kp="));display.println(m_cachedKp,3);
  display.print(F("Kd="));display.println(m_cachedKd,3);
  display.display();
}