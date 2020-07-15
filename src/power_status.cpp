#include "power_status.h"

void PowerStatus::pluggedInSummary() {
  snprintf(display_buffer, sizeof(display_buffer),
      "%i%%%s %4.1fmA %4.1fmA",
      watch->power->getBattPercentage(),
      "",
      0.0, 0.0
  );
      // charging() ? "+" : "",
      // battery_current.average(),
      // vbus_current.average()
}

void PowerStatus::batterySummary() {
  snprintf(display_buffer, sizeof(display_buffer),
      "%i%% %4.1fmA",
      watch->power->getBattPercentage(),
      0.0
  );
      // battery_current.average()
}

void PowerStatus::run() {
  // if (low_power)
  if (false)
    delay_time = 1000;
  else
    delay_time = 250;

  refresh_display = true;
}

void PowerStatus::display() {
  // if (pluggedIn()) pluggedInSummary();
  if (true) pluggedInSummary();
  else batterySummary();

  screen->setTextPadding(screen->textWidth(display_buffer));
  screen->drawString(display_buffer, 0, 0, 2);
}
