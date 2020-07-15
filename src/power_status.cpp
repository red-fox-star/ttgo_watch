#include "power_status.h"

void PowerStatus::pluggedInSummary() {
  snprintf(display_buffer, sizeof(display_buffer),
      "%i%%%s %4.1fmA %4.1fmA",
      power->batteryPercentage(),
      power->charging() ? "+" : "",
      power->batteryCurrent(),
      power->vbusCurrent()
  );
}

void PowerStatus::batterySummary() {
  snprintf(display_buffer, sizeof(display_buffer),
      "%i%% %4.1fmA",
      watch->power->getBattPercentage(),
      power->batteryCurrent()
  );
}

void PowerStatus::run() {
  if (power->lowPower())
    delay_time = 1000;
  else
    delay_time = 250;

  refresh_display = true;
}

void PowerStatus::display() {
  if (power->pluggedIn()) pluggedInSummary();
  else batterySummary();

  screen->setTextPadding(screen->textWidth(display_buffer));
  screen->drawString(display_buffer, 0, 0, 2);
}
