#include "EventBus.h"
#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(int id, int pinA, int pinB, int pinBtn, EventBus* pEventBus) :
  m_enc(pinA, pinB),
  m_pinBtn(pinBtn),
  m_nId(id),
  m_pEventBus(pEventBus),
  m_tButtonSaturationTimer(0)
{
  pinMode(m_pinBtn, INPUT); // setup rotary encoder push-button
}

void RotaryEncoder::Poll()
{
  if (digitalReadFast(m_pinBtn)) {
    unsigned long timeVal = millis();
    if (timeVal - m_tButtonSaturationTimer > 500) { // 500ms saturation on button trigger to debounce
      m_tButtonSaturationTimer = timeVal;
      bool payload = true; // dummy payload
      m_pEventBus->Publish(m_nId, EEventType::RotaryEncoderButton, &payload);
    }
  }
  
  long encPos = m_enc.readAndReset();
  if (encPos != 0) {
    float payload = (float)encPos/80;
    m_pEventBus->Publish(m_nId, EEventType::RotaryEncoderPosition, &payload); // 80 positions in a complete single turn
  }
}