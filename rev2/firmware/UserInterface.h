#ifndef __USERINTERFACE_H_
#define __USERINTERFACE_H_

#include "EventBus.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

typedef enum _EUIState {
  Info,
  AdjustPID
} EUIState;

class UserInterface : public INotifiable
{
public:
  UserInterface(EventBus* pEventBus);
  virtual void OnEventNotify(int sourceId, EEventType type, const void* eventData) override;
  bool Initialise();

private:
  void DrawPIDScreen();
  
  EUIState m_uiState;

  // cached pid values for display
  float m_cachedKp;
  float m_cachedKd;

  EventBus* m_pEventBus;
};

#endif