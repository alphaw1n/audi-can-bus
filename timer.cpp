#include <Arduino.h>
#include "timer.h"

void Timer::start(long unsigned int toStart_ms)
{
//        Serial.println("timer_::start()");
    each_ms = toStart_ms;
    alarm_after_ms = each_ms + millis();
}

bool Timer::isActive()
{
    if ( alarm_after_ms <= millis() )
    {
        alarm_after_ms = each_ms + millis();
        return true;
    }
    else
    {
        return false;
    }
}
