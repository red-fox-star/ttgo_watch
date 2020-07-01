#include "actor.h"
#include "moving_average.h"

class PowerStatus : public Actor {
  public:
    bool ready();
    void run();
    void init();

  private:
    long unsigned int last_run = 0;
    int delta = 0;
    MovingAverage<float, 100, 25> battery_current;
    MovingAverage<float, 100, 25> vbus_current;
};
