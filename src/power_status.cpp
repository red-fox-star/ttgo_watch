#include "power_status.h"

void PowerStatus::init() {
  watch->power->adc1Enable(
      AXP202_VBUS_VOL_ADC1
      | AXP202_VBUS_CUR_ADC1
      | AXP202_BATT_CUR_ADC1
      | AXP202_BATT_VOL_ADC1
  , true);
}

bool PowerStatus::ready() {
  battery_current.insert(watch->power->getBattDischargeCurrent());
  vbus_current.insert(watch->power->getVbusCurrent());

  delta = timestamp() - last_run;
  if (delta >= 0 && delta < 500) {
    return false;
  }

  last_run = timestamp();
  return true;
}

void PowerStatus::run() {
  char display[255];

  // if (watch->power->isChargeing()) { // this doesn't work
  snprintf(display, sizeof(display),
      "%i%% %3.1fV %4.1fmA / %3.1fV %4.1fmA",
      watch->power->getBattPercentage(),
      watch->power->getBattVoltage() / 1000,
      battery_current.average(),
      watch->power->getVbusVoltage() / 1000,
      vbus_current.average()
  );

  screen->fillRect(0, 0, 240, 40, TFT_BLACK);
  watch->eTFT->drawString(display, 0, 0, 2);

  snprintf(display, sizeof(display), "%i - %lu", delta, millis());
  watch->eTFT->drawString(display, 0, 20, 2);
}

