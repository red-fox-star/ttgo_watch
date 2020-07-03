#include "clock.h"

void Clock::init() {
  watch->power->adc1Enable(
      AXP202_VBUS_VOL_ADC1
      | AXP202_VBUS_CUR_ADC1
      | AXP202_BATT_CUR_ADC1
      | AXP202_BATT_VOL_ADC1
  , true);

  x_middle = screen->width() / 2;
  y_middle = screen->height() / 2;

  font_height = screen->fontHeight();
  y_position = y_middle - (font_height / 2);
}

bool Clock::ready() {
  time = watch->rtc->getDateTime();
  if (last_second == time.second)
    return false;

  last_second = time.second;
  return true;
}

void Clock::run() {
  char output[5];
  if (time.second == 0) {
    screen->fillRect(0, 0, 240, 240, TFT_BLACK);
  }

  snprintf(output, sizeof(output), "%02u%02u", time.hour, time.minute);

  int16_t text_width = screen->textWidth(output);
  int16_t x_position = x_middle - (text_width / 2);

  screen->fillRect(
      x_position, y_position,
      text_width, font_height,
      TFT_BLACK
  );

  screen->drawString(output, x_position, y_position);
}
