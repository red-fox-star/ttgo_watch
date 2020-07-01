#include "actor.h"

class SleeperAgent : public Actor {
  public:
    bool ready();
    void run();
    void init();
    static bool asleep();
    void powerUp();

  private:
    static bool low_power;

    long unsigned int last_touch = 0;
    long unsigned int wake_time = 0;
    long unsigned int sleep_time = 0;

    bool should_wake = false;

    bool checkTouch();
    void powerDown();
};
