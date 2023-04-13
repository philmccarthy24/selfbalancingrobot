#ifndef __EVENTBUS_H_
#define __EVENTBUS_H_

typedef enum _EEventType {
  RotaryEncoderPosition,
  RotaryEncoderButton,
  AHRSUpdate,
  TimerTimeout
} EEventType;

class INotifiable
{
public:
  virtual void OnEventNotify(int sourceId, EEventType type, const void* eventData) = 0;
};

// very simple event broker class to decouple dependencies
class EventBus
{
public:
  EventBus();
  void Subscribe(INotifiable* pSubscriber); // TODO: improvement would be to allow subscriptions per topic
  void Publish(int sourceId, EEventType type, const void* eventData);

private:
  INotifiable* m_subscribers[10];
  int m_numSubscribers;
};

#endif