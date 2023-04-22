#ifndef __EVENTBUS_H_
#define __EVENTBUS_H_

typedef enum _EEventType {
  RotaryEncoderPosition,
  RotaryEncoderButton,
  AHRSUpdate,
  TimerTimeout, // id 0 = main 100hz timer
  SettingsSync, // id 0 = MotionController source with float[3] payload, id 1 = UserInterface with float[3] payload
  StoreData, // id 0 = UserInterface to MC to make it store current pid setting. No payload
  ZeroTilt // id 0 = UserInterface source. Also makes AHRS persist new tilt offset. For initial balancing setup
} EEventType;

class INotifiable
{
public:
  virtual void OnEventNotify(int sourceId, EEventType type, const void* eventData) = 0;
};

// very simple event broker class to decouple dependencies.
// need to be careful not to trigger infinite loops / stack overflows.
class EventBus
{
public:
  EventBus();
  void Subscribe(INotifiable* pSubscriber); // TODO: improvement would be to allow subscriptions per topic
  void Publish(int sourceId, EEventType type, const void* eventData); // TODO: improvement would be to queue publish calls and service them in sep loops.

private:
  INotifiable* m_subscribers[10];
  int m_numSubscribers;
};

#endif