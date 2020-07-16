#pragma once
#include "actor.h"

class Clock : public Actor {
  public:
    bool ready();
    void run();
    void init();
    void display();

    const uint32_t displayIdentifier() {
      return 0x4;
    }


  private:
    bool redisplay = false;

    RTC_Date time;
    int16_t x_middle, y_middle;

    int16_t hours_x = 0;
    int16_t hours_y = 0;
    int16_t minutes_x = 0;
    int16_t minutes_y = 0;
    int16_t hours_minutes_space = 0;
    int16_t seconds_x = 0;
    int16_t seconds_y = 0;
    int16_t seconds_space = 0;

    int16_t hours_font = 8;
    int16_t minutes_font = 8;
    int16_t seconds_font = 4;

    int16_t old_hour = 24;
    int16_t old_minute = 61;
    int16_t old_second = 61;
};
