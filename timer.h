class Timer
{
  private:
    long unsigned int alarm_after_ms;
    long unsigned int each_ms;

  public:
    Timer() : each_ms(0), alarm_after_ms(0) {}
    ~Timer() {}

    void start(long unsigned int toStart_ms);
    bool isActive();
};
