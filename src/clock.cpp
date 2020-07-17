#include "clock.h"

void Clock::init() {
  x_middle = screen->width() / 2;
  y_middle = screen->height() / 2;

  int16_t hours_half_height = screen->fontHeight(hour.font) / 2;
  int16_t seconds_width_zero = screen->textWidth("0", second.font);
  hours_minutes_space = 10;
  second.space = seconds_width_zero / 2;

  hour.x = x_middle - screen->textWidth("00", hour.font) - (hours_minutes_space / 2);
  hour.y = y_middle - hours_half_height;

  minute.x = x_middle + (hours_minutes_space / 2);
  minute.y = hour.y;

  second.x = x_middle - seconds_width_zero - (second.space / 2);
  second.y = y_middle + hours_half_height + 10;

  date.x = 0;
  date.y = screen->height() - (screen->fontHeight(second.font) + 2);
}

void Clock::run() {
  time = watch->rtc->getDateTime();
  delay_time = 250;

  if (wakeUpRun()) redisplay = true;

  refresh_display = redisplay || time.second != old_second;
}

void Clock::display() {
  int16_t position_x;

  redisplay = redisplay || time.second % 10 == 0;

  if (redisplay || time.day != old_day) {
    old_day = time.day;
    position_x = date.x;
    position_x += screen->drawNumber(time.month, position_x, date.y, date.font);
    position_x += screen->drawChar('.', position_x, date.y, date.font);
    position_x += screen->drawNumber(time.day, position_x, date.y, date.font);
  }

  // hour
  if (redisplay || old_hour != time.hour) {
    old_hour = time.hour;
    position_x = hour.x;
    screen->setTextPadding(screen->textWidth("0", hour.font));
    if (time.hour < 10) position_x += screen->drawChar('0', position_x, hour.y, hour.font);
    position_x += screen->drawNumber(time.hour, position_x, hour.y, hour.font);
  }

  // minute
  if (redisplay || old_minute != time.minute) {
    old_minute = time.minute;
    position_x = minute.x;
    screen->setTextPadding(screen->textWidth("0", minute.font));
    if (time.minute < 10) position_x += screen->drawChar('0', position_x, minute.y, minute.font);
    position_x += screen->drawNumber(time.minute, position_x, minute.y, minute.font);
  }

  // seconds
  old_second = time.second;
  position_x = second.x;
  screen->setTextPadding(screen->textWidth("0", second.font));
  if (time.second < 10) {
    position_x += screen->drawChar('0', position_x, second.y, second.font);
    position_x += second.space;
    position_x += screen->drawNumber(time.second, position_x, second.y, second.font);
  } else {
    position_x += screen->drawNumber(time.second / 10, position_x, second.y, second.font);
    position_x += second.space;
    position_x += screen->drawNumber(time.second % 10, position_x, second.y, second.font);
  }

  redisplay = false;
}
