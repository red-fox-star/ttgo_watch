#include "power_status.h"

TTGOClass * PowerManager::watch = nullptr;
TFT_eSPI * PowerManager::screen = nullptr;

void PowerManager::interrupt(){ _read_irq = true; }

bool PowerManager::lowPower() { return low_power; }
bool PowerManager::pluggedIn() { return _plugged_in; }
bool PowerManager::charging() { return _charging; }
void PowerManager::pluggedIn(bool state) { _plugged_in = state; }
void PowerManager::charging(bool state) { _charging = state; }

int PowerManager::batteryPercentage() {
  int actual_percentage = watch->power->getBattPercentage();

  if (pluggedIn()) {
    if (actual_percentage < battery_percentage) {
      return battery_percentage;
    } else {
      return battery_percentage = actual_percentage;
    }
  } else {
    if (actual_percentage > battery_percentage) {
      return battery_percentage;
    } else {
      return battery_percentage = actual_percentage;
    }
  }
}

float PowerManager::batteryCurrent() {
  return battery_current.average();
}

float PowerManager::vbusCurrent() {
  return vbus_current.average();
}

void PowerManager::powerDown() {
  q_message_ln("powering down");
  watch->closeBL();
  watch->displaySleep();

  low_power = true;
  sleep_time = millis();
}

void PowerManager::powerUp() {
  q_message_ln("powering up");
  watch->displayWakeup();
  watch->openBL();

  wake_time = last_interaction = millis();
  low_power = false;
}

void PowerManager::suspend() {
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

  watch->power->setPowerOutPut(
      AXP202_EXTEN
      | AXP202_DCDC2
      | AXP202_LDO3
      | AXP202_LDO4
  , AXP202_OFF);

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
  battery_percentage = watch->power->getBattPercentage();
}

void PowerManager::readIRQ() {
  watch->power->readIRQ();

  last_interaction = millis();

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

void PowerManager::tryToPowerUp() {
  if (last_touch > sleep_time) {
    powerUp();
  }
}

void PowerManager::tryToPowerDown() {
  if (pluggedIn()) return;
  if (millis() - last_interaction > power_down_delay) {
    powerDown();
  }
}

void PowerManager::tryToSuspend() {
  if (millis() - last_interaction > suspend_delay) {
    suspend();
  }
}

void PowerManager::run() {
  if (_read_irq) readIRQ();
  checkTouch();

  if (low_power) {
    tryToPowerUp();
    if (low_power) tryToSuspend();
  } else {
    tryToPowerDown();
  }

  if (millis() - last_logged > 100) {
    last_logged = millis();
    logPower();
  }
}

void PowerManager::logPower() {
  battery_current.insert(watch->power->getBattDischargeCurrent());
  vbus_current.insert(watch->power->getVbusCurrent());
}

void powerManagementTask(void* object) {
  PowerManager* power_manager = (PowerManager *) object;

  power_manager->init();

  for (;;) {
    power_manager->run();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  q_message_ln("deleting the power management task!");
  vTaskDelete(NULL);
}

void runPowerManagement(PowerManager * power_manager) {
  xTaskCreate(powerManagementTask, "power_manager", 10000, (void *) power_manager, 30, NULL);
}
