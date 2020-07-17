#pragma once
#include "actor.h"

class DigitMetric {
  public:
    DigitMetric(int16_t _font) : font(_font), space(0), x(0), y(0) {};

    int16_t font = 1;
    int16_t space = 0;
    int16_t x = 0;
    int16_t y = 0;
};

class Clock : public Actor {
  public:
    Clock() : date{4}, hour{8}, minute{8}, second{4} { };

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

    DigitMetric date;
    DigitMetric hour;
    DigitMetric minute;
    DigitMetric second;

    int16_t hours_minutes_space = 0;

    // int16_t hours_x = 0;
    // int16_t hours_y = 0;
    // int16_t minutes_x = 0;
    // int16_t minutes_y = 0;
    // int16_t seconds_x = 0;
    // int16_t seconds_y = 0;
    // int16_t seconds_space = 0;
    // int16_t hours_font = 8;
    // int16_t minutes_font = 8;
    // int16_t seconds_font = 4;

    int16_t old_day = 31;
    int16_t old_hour = 24;
    int16_t old_minute = 61;
    int16_t old_second = 61;
};
