#include <Arduino.h>
#include "timer.h"

Timer::Timer()
{
  m_each_ms = 0;
  m_alarm_after_ms = 0;
  m_adjust = 0;
};

void Timer::start(long unsigned int toStart_ms)
{
  m_each_ms = toStart_ms;
  m_adjust = m_each_ms;
  m_alarm_after_ms = m_each_ms + millis();
}

bool Timer::isActive()
{
  if ( m_alarm_after_ms <= millis() )
  {
    //m_alarm_after_ms = ( m_each_ms - ( m_adjust - m_each_ms ) ) + millis();
    m_alarm_after_ms = m_each_ms  + millis();
    return true;
  }
  else
  {
    return false;
  }
}

void Timer::setAdjust(long unsigned int adjust)
{
  m_adjust = adjust;
}


