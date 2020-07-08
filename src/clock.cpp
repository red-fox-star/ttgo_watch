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

void Clock::run() {
  time = watch->rtc->getDateTime();
  refresh_display = time.second != last_second;
}

void Clock::display() {
  Serial.printf("clock display (%i)\n", displayIdentifier());
  char output[5];
  time = watch->rtc->getDateTime();
  last_second = time.second;

  snprintf(output, sizeof(output), "%02u%02u", time.hour, time.minute);
  int16_t text_width = screen->textWidth(output);
  int16_t x_position = x_middle - (text_width / 2);

  if (time.second == 0) {
    screen->fillRect(
        x_position, y_position,
        text_width, font_height,
        TFT_BLACK
    );
  }


  screen->drawString(output, x_position, y_position);

  snprintf(output, sizeof(output), "%02u", time.second);
  screen->drawString(output, x_middle, y_position + font_height + 5);
  delay_time = 1000;
}
