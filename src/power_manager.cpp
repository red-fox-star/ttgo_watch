#include "power_status.h"

bool PowerManager::low_power = false;

void PowerManager::interrupt(){ _read_irq = true; }

bool PowerManager::asleep() { return low_power; }
bool PowerManager::pluggedIn() { return _plugged_in; }
bool PowerManager::charging() { return _charging; }
void PowerManager::pluggedIn(bool state) { _plugged_in = state; }
void PowerManager::charging(bool state) { _charging = state; }

void PowerManager::powerDown() {
  q_message_ln("powering down");
  watch->closeBL();
  watch->displaySleep();

  low_power = true;
  sleep_time = millis();
}

void PowerManager::powerUp() {
  q_message_ln("waking up");
  watch->displayWakeup();
  watch->openBL();

  wake_time = last_interaction = millis();
  low_power = false;
}

void PowerManager::sleepHarder() {
  esp_sleep_enable_gpio_wakeup();
  q_message_ln("starting sleep");
  screen->fillScreen(TFT_BLACK);
  delay(75);
  esp_light_sleep_start();
  q_message_ln("back from sleep");
}

bool PowerManager::checkTouch() {
  if (watch->touch->touched() > 0) {
    last_touch = last_interaction = millis();
    return true;
  }

  return false;
}

void PowerManager::init() {
  watch = TTGOClass::getWatch();
  screen = watch->eTFT;

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

void PowerManager::readIRQ() {
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

void PowerManager::sleepOrWake() {
  checkTouch();

  if (low_power) {
    tryToWake();
    if (low_power) tryToSleepHarder();

  } else {
    tryToSleep();
  }
}

void PowerManager::tryToWake() {
  if (last_touch > sleep_time) {
    powerUp();
  }
}

void PowerManager::tryToSleep() {
  if (pluggedIn()) return;
  if (millis() - last_interaction > 11000) {
    powerDown();
  }
}

void PowerManager::tryToSleepHarder() {
  if (millis() - last_interaction > 15000) {
    sleepHarder();
  }
}

void PowerManager::run() {
  if (_read_irq) readIRQ();

  sleepOrWake();
}

void PowerManager::logPower() {
  battery_current.insert(watch->power->getBattDischargeCurrent());
  vbus_current.insert(watch->power->getVbusCurrent());
}
