#ifndef __ROTARYENCODER_H_
#define __ROTARYENCODER_H_

#include <Encoder.h>

// forward declaration
class EventBus;

// rotary encoder event source
class RotaryEncoder
{
  public:
  RotaryEncoder(int id, int pinA, int pinB, int pinBtn, EventBus* pEventBus);

  void Poll();

  private:
  Encoder m_enc;
  int m_pinBtn;
  int m_nId;
  EventBus* m_pEventBus;
  unsigned long m_tButtonSaturationTimer;
};

#endif