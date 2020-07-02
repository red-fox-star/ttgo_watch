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
    long unsigned int last_run = 0;
    long unsigned int last_logged = 0;
    long unsigned int delta = 0;
    bool _plugged_in = false;
    bool _charging = false;
    bool _read_irq = false;

    long unsigned int last_touch = 0;
    long unsigned int wake_time = 0;
    long unsigned int sleep_time = 0;
    static bool low_power;

    bool should_wake = false;

    void readIRQ();
    void pluggedIn(bool state);
    void charging(bool state);
    bool checkTouch();
};
