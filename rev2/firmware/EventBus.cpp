#include "EventBus.h"

EventBus::EventBus() :
m_subscribers {},
m_numSubscribers(0)
{
}

void EventBus::Subscribe(INotifiable* pSubscriber)
{
  m_subscribers[m_numSubscribers] = pSubscriber;
  m_numSubscribers++;
}

void EventBus::Publish(int sourceId, EEventType type, const void* eventData)
{
  for (int i = 0; i < m_numSubscribers; i++)
  {
    m_subscribers[i]->OnEventNotify(sourceId, type, eventData);
  }
}