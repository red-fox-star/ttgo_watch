#include "clock.h"

void Clock::init() {
  screen->fillScreen(TFT_BLACK);
  screen->setTextColor(TFT_WHITE, TFT_BLACK);

  watch->power->adc1Enable(
      AXP202_VBUS_VOL_ADC1
      | AXP202_VBUS_CUR_ADC1
      | AXP202_BATT_CUR_ADC1
      | AXP202_BATT_VOL_ADC1
  , true);

  x_middle = screen->width() / 2;
  y_middle = screen->height() / 2;

  int16_t hours_half_height = screen->fontHeight(hours_font) / 2;

  hours_x = x_middle - screen->textWidth("00", hours_font);
  hours_y = y_middle - hours_half_height;

  minutes_x = x_middle;
  minutes_y = hours_y;

  seconds_x = x_middle - screen->textWidth("0", seconds_font);
  seconds_y = y_middle + hours_half_height + 10;
}

void Clock::run() {
  time = watch->rtc->getDateTime();
  delay_time = 250;
  refresh_display = time.second != old_second;
}

void Clock::display() {
  int16_t position_x;

  // hour
  if (old_hour != time.hour || time.second % 10 == 0) {
    old_hour = time.hour;
    position_x = hours_x;
    screen->setTextPadding(screen->textWidth("0", hours_font));
    if (time.hour < 10) position_x += screen->drawChar('0', position_x, hours_y, hours_font);
    position_x += screen->drawNumber(time.hour, position_x, hours_y, hours_font);
  }

  // minute
  if (old_minute != time.minute || time.second % 10 == 0) {
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
  if (time.second < 10) position_x += screen->drawChar('0', position_x, seconds_y, seconds_font);
  position_x += screen->drawNumber(time.second, position_x, seconds_y, seconds_font);
}
