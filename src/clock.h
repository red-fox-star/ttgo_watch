#include "actor.h"

class Clock : public Actor {
  public:
    bool ready();
    void run();
    void init();

  private:
    RTC_Date time;
    int last_second = 61;
    int16_t x_middle, y_middle;
    int16_t x_position, y_position;
    int16_t font_height;
};
