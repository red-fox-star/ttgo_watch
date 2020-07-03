#include "actor.h"
#include "moving_average.h"

class PowerStatus : public Actor {
  public:
    bool ready();
    void run();
    void init();
    void interrupt();

    bool pluggedIn();
    bool charging();

    void powerDown();
    void powerUp();

    static bool asleep();


  private:
    // calculating and displaying the runtime statistics
    long unsigned int last_run = 0;
    bool _plugged_in = false;
    bool _charging = false;
    bool _read_irq = false;
    MovingAverage<float, 100> battery_current;
    MovingAverage<float, 100> vbus_current;
    long unsigned int last_logged = 0;

    void logPower();
    void pluggedInSummary(char * buffer, int len);
    void batterySummary(char * buffer, int len);

    // managing the low_power/high_power state
    long unsigned int last_touch = 0;
    long unsigned int wake_time = 0;
    long unsigned int sleep_time = 0;
    static bool low_power;
    bool should_wake = false;

    void readIRQ();
    void charging(bool state);
    void pluggedIn(bool state);
    bool checkTouch();
    void touchWake();
};
