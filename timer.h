#ifndef __HEADER_TIMER
#define __HEADER_TIMER 1

class Timer
{
  private:
    long unsigned int m_alarm_after_ms;
    long unsigned int m_each_ms;
    long unsigned int m_adjust;
    bool m_inited;

  public:
    Timer();
    ~Timer() {}

    void start(long unsigned int toStart_ms);
    bool isActive();
    void setAdjust(long unsigned int adjust);
    bool isInited();
};

#endif;
