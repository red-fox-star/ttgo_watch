#include "power_status.h"

bool PowerStatus::low_power = false;

void PowerStatus::interrupt(){ _read_irq = true; }

bool PowerStatus::asleep() { return low_power; }
bool PowerStatus::pluggedIn() { return _plugged_in; }
bool PowerStatus::charging() { return _charging; }
void PowerStatus::pluggedIn(bool state) { _plugged_in = state; }
void PowerStatus::charging(bool state) { _charging = state; }

void PowerStatus::powerDown() {
  // setCpuFrequencyMhz(40);
  watch->closeBL();
  watch->displaySleep();

  low_power = true;
  sleep_time = timestamp();
}

void PowerStatus::powerUp() {
  // setCpuFrequencyMhz(240);
  // watch->bl->adjust(255);
  watch->displayWakeup();
  watch->openBL();

  wake_time = timestamp();
  low_power = false;
}

bool PowerStatus::checkTouch() {
  if (watch->touch->touched() > 0) {
    last_touch = timestamp();
    return true;
  }

  return false;
}

void PowerStatus::init() {
  watch->power->adc1Enable(
      AXP202_BATT_VOL_ADC1
      | AXP202_BATT_CUR_ADC1
      | AXP202_VBUS_CUR_ADC1
  , AXP202_ON);

  watch->power->adc1Enable(
      AXP202_VBUS_VOL_ADC1
  , AXP202_OFF);

  watch->power->enableIRQ(
      AXP202_VBUS_REMOVED_IRQ
      | AXP202_VBUS_CONNECT_IRQ
      | AXP202_CHARGING_IRQ
      | AXP202_CHARGING_FINISHED_IRQ
      | AXP202_PEK_LONGPRESS_IRQ
      | AXP202_PEK_SHORTPRESS_IRQ
  , AXP202_ON);

  watch->power->clearIRQ();

  pluggedIn(watch->power->isVBUSPlug());
  charging(watch->power->isChargeingEnable());
}

void PowerStatus::readIRQ() {
  watch->power->readIRQ();

  if (watch->power->isVbusPlugInIRQ())   pluggedIn(true);
  if (watch->power->isVbusRemoveIRQ())   pluggedIn(false);
  if (watch->power->isChargingIRQ())     charging(true);
  if (watch->power->isChargingDoneIRQ()) charging(false);

  if (watch->power->isPEKShortPressIRQ()) {
    if (low_power) powerUp();
    else powerDown();
  }

  watch->power->clearIRQ();
}

void PowerStatus::touchWake() {
  if (low_power) {
    checkTouch();

    if (last_touch > sleep_time) {
      powerUp();
    }
  } else {
    if (timestamp() - wake_time > 11000) {
      powerDown();
    }
  }
}

void PowerStatus::logPower() {
  if (timestamp() - last_logged < 100) return;
  last_logged = timestamp();
  battery_current.insert(watch->power->getBattDischargeCurrent());
  vbus_current.insert(watch->power->getVbusCurrent());
}

bool PowerStatus::ready() {
  if (_read_irq) readIRQ();
  touchWake();
  logPower();
  if (low_power) return false;

  unsigned long int delta = timestamp() - last_run;
  if (delta >= 0 && delta < 500) {
    return false;
  }

  last_run = timestamp();
  return true;
}

void PowerStatus::pluggedInSummary(char * buffer, int len) {
  snprintf(buffer, len,
      "%i%%%s %4.1fmA %4.1fmA",
      watch->power->getBattPercentage(),
      charging() ? "+" : "",
      battery_current.average(),
      vbus_current.average()
  );
}

void PowerStatus::batterySummary(char * buffer, int len) {
  snprintf(buffer, len,
      "%i%% %4.1fmA",
      watch->power->getBattPercentage(),
      battery_current.average()
  );
}

void PowerStatus::run() {
  char display[255];

  if (pluggedIn()) pluggedInSummary(display, sizeof(display));
  else batterySummary(display, sizeof(display));

  screen->fillRect(0, 0, 240, 20, TFT_BLACK);
  watch->eTFT->drawString(display, 0, 0, 2);
}

