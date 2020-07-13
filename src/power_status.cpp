#include "power_status.h"

bool PowerStatus::low_power = false;

void PowerStatus::interrupt(){ _read_irq = true; }

bool PowerStatus::asleep() { return low_power; }
bool PowerStatus::pluggedIn() { return _plugged_in; }
bool PowerStatus::charging() { return _charging; }
void PowerStatus::pluggedIn(bool state) { _plugged_in = state; }
void PowerStatus::charging(bool state) { _charging = state; }

void PowerStatus::powerDown() {
  watch->closeBL();
  watch->displaySleep();

  low_power = true;
  sleep_time = millis();
}

void PowerStatus::powerUp() {
  watch->displayWakeup();
  watch->openBL();

  wake_time = millis();
  low_power = false;
}

bool PowerStatus::checkTouch() {
  if (watch->touch->touched() > 0) {
    last_touch = millis();
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
    if (millis() - wake_time > 11000) {
      powerDown();
    }
  }
}

void PowerStatus::logPower() {
  if (millis() - last_logged < 100) return;
  last_logged = millis();
  battery_current.insert(watch->power->getBattDischargeCurrent());
  vbus_current.insert(watch->power->getVbusCurrent());
}

void PowerStatus::pluggedInSummary() {
  snprintf(display_buffer, sizeof(display_buffer),
      "%i%%%s %4.1fmA %4.1fmA",
      watch->power->getBattPercentage(),
      charging() ? "+" : "",
      battery_current.average(),
      vbus_current.average()
  );
}

void PowerStatus::batterySummary() {
  snprintf(display_buffer, sizeof(display_buffer),
      "%i%% %4.1fmA",
      watch->power->getBattPercentage(),
      battery_current.average()
  );
}

void PowerStatus::run() {
  if (_read_irq) readIRQ();

  touchWake();
  logPower();

  // if (low_power)
  //   delay_time = 1000;
  // else
  //   delay_time = 500;
  delay_time = 250;

  refresh_display = last_run ++ > 8;
}

void PowerStatus::display() {
  last_run = 0;
  if (pluggedIn()) pluggedInSummary();
  else batterySummary();

  screen->fillRect(0, 0, 240, 20, TFT_BLACK);
  watch->eTFT->drawString(display_buffer, 0, 0, 2);
}
