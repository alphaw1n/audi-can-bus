class Timer
{
  private:
    long unsigned int m_alarm_after_ms;
    long unsigned int m_each_ms;
    long unsigned int m_adjust;

  public:
    Timer();
    ~Timer() {}

    void start(long unsigned int toStart_ms);
    bool isActive();
    void setAdjust(long unsigned int adjust);
};
