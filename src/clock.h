#pragma once
#include "actor.h"
#include "displayable.h"

class Clock : public Actor, public Displayable {
  public:
    bool ready();
    void run();
    void init();
    void display();

    const uint32_t displayIdentifier() {
      return 0x4;
    }

  private:
    RTC_Date time;
    int last_second = 61;
    int16_t x_middle, y_middle;
    int16_t x_position, y_position;
    int16_t font_height;
};
