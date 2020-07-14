#include "power_status.h"

bool PowerStatus::low_power = false;

void PowerStatus::interrupt(){ _read_irq = true; }

bool PowerStatus::asleep() { return low_power; }
bool PowerStatus::pluggedIn() { return _plugged_in; }
bool PowerStatus::charging() { return _charging; }
void PowerStatus::pluggedIn(bool state) { _plugged_in = state; }
void PowerStatus::charging(bool state) { _charging = state; }

void PowerStatus::powerDown() {
  q_message_ln("powering down");
  watch->closeBL();
  watch->displaySleep();

  low_power = true;
  sleep_time = millis();
}

void PowerStatus::powerUp() {
  q_message_ln("waking up");
  watch->displayWakeup();
  watch->openBL();

  wake_time = last_interaction = millis();
  low_power = false;
}

void PowerStatus::sleepHarder() {
  esp_sleep_enable_gpio_wakeup();
  q_message_ln("starting sleep");
  delay(150);
  esp_light_sleep_start();
  q_message_ln("back from sleep");
}

bool PowerStatus::checkTouch() {
  if (watch->touch->touched() > 0) {
    last_touch = last_interaction = millis();
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

  gpio_wakeup_enable((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);

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

void PowerStatus::sleepOrWake() {
  checkTouch();

  if (low_power) {
    tryToWake();
    if (low_power) tryToSleepHarder();

  } else {
    tryToSleep();
  }
}

void PowerStatus::tryToWake() {
  if (last_touch > sleep_time) {
    powerUp();
  }
}

void PowerStatus::tryToSleep() {
  if (pluggedIn()) return;
  if (millis() - last_interaction > 11000) {
    powerDown();
  }
}

void PowerStatus::tryToSleepHarder() {
  if (millis() - last_interaction > 22000) {
    sleepHarder();
  }
}

void PowerStatus::logPower() {
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

  sleepOrWake();
  logPower();

  if (low_power)
    delay_time = 1000;
  else
    delay_time = 250;

  refresh_display = !low_power && last_run ++ > 8;
}

void PowerStatus::display() {
  last_run = 0;
  if (pluggedIn()) pluggedInSummary();
  else batterySummary();

  screen->fillRect(0, 0, 240, 20, TFT_BLACK);
  watch->eTFT->drawString(display_buffer, 0, 0, 2);
}
