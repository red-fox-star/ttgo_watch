#include "clock.h"

void Clock::init() {
  x_middle = screen->width() / 2;
  y_middle = screen->height() / 2;

  int16_t hours_half_height = screen->fontHeight(hours_font) / 2;
  int16_t seconds_width_zero = screen->textWidth("0", seconds_font);
  hours_minutes_space = 10;
  seconds_space = seconds_width_zero / 2;

  hours_x = x_middle - screen->textWidth("00", hours_font) - (hours_minutes_space / 2);
  hours_y = y_middle - hours_half_height;

  minutes_x = x_middle + (hours_minutes_space / 2);
  minutes_y = hours_y;

  seconds_x = x_middle - seconds_width_zero - (seconds_space / 2);
  seconds_y = y_middle + hours_half_height + 10;
}

void Clock::run() {
  time = watch->rtc->getDateTime();
  delay_time = 250;
  refresh_display = time.second != old_second;
}

void Clock::display() {
  int16_t position_x;

  if (time.second % 10 == 0) old_hour = old_minute = 61;

  // hour
  if (old_hour != time.hour) {
    old_hour = time.hour;
    position_x = hours_x;
    screen->setTextPadding(screen->textWidth("0", hours_font));
    if (time.hour < 10) position_x += screen->drawChar('0', position_x, hours_y, hours_font);
    position_x += screen->drawNumber(time.hour, position_x, hours_y, hours_font);
  }

  // minute
  if (old_minute != time.minute) {
    old_minute = time.minute;
    position_x = minutes_x;
    screen->setTextPadding(screen->textWidth("0", minutes_font));
    if (time.minute < 10) position_x += screen->drawChar('0', position_x, minutes_y, minutes_font);
    position_x += screen->drawNumber(time.minute, position_x, minutes_y, minutes_font);
  }

  // seconds
  old_second = time.second;
  position_x = seconds_x;
  screen->setTextPadding(screen->textWidth("0", seconds_font));
  if (time.second < 10) {
    position_x += screen->drawChar('0', position_x, seconds_y, seconds_font);
    position_x += seconds_space;
    position_x += screen->drawNumber(time.second, position_x, seconds_y, seconds_font);
  } else {
    position_x += screen->drawNumber(time.second / 10, position_x, seconds_y, seconds_font);
    position_x += seconds_space;
    position_x += screen->drawNumber(time.second % 10, position_x, seconds_y, seconds_font);
  }
}
