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
  int delta = timestamp() - last_run;
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
      watch->power->getBattDischargeCurrent(),
      watch->power->getVbusVoltage() / 1000,
      watch->power->getVbusCurrent()
  );

  screen->fillRect(0, 0, 240, 20, TFT_BLACK);
  watch->eTFT->drawString(display, 0, 0, 2);
}

