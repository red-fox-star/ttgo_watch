#define LILYGO_TWATCH_2020_V1

#include <TTGO.h>
#include <WiFi.h>

#include "serial_message_queue.h"
#include "power_manager.h"

#include "pong.h"
#include "clock.h"
#include "power_status.h"
#include "display.h"

TTGOClass *watch;

Pong display_pong;
Clock display_clock;
PowerStatus display_power;
PowerManager power_manager;
Display display;
TaskHandle_t display_task;

bool low_power = false;

void ntpRead() {
  configTzTime("MST7MDT,M3.2.0/02:00:00,M11.1.0/02:00:00", "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org");

  static struct tm timeinfo;
  getLocalTime(&timeinfo, 10000);
  Serial.println(&timeinfo, "Setting time to: %B %d %Y %H:%M:%S (%A)");

  watch->rtc->syncToRtc();
}

void connectWifi() {
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
      q_message_ln("WiFi connected");
      ntpRead();
  }, WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);

  WiFi.enableSTA(true);
  WiFi.begin();
}

void setup(void) {
  runSerialMessenger();

  watch = TTGOClass::getWatch();
  watch->begin();

  watch->rtc->check();
  watch->bl->adjust(150);

  watch->power->setPowerOutPut(
      AXP202_EXTEN
      | AXP202_DCDC2
      | AXP202_LDO3
      | AXP202_LDO4
  , AXP202_OFF);

  watch->eTFT->fillScreen(TFT_BLACK);
  watch->eTFT->setTextColor(TFT_WHITE, TFT_BLACK);
  watch->eTFT->setTextFont(8);
  watch->openBL();

  pinMode(AXP202_INT, INPUT);
  attachInterrupt(AXP202_INT, [] {
      power_manager.interrupt();
  }, FALLING);

  WiFi.mode(WIFI_OFF);

  display_pong.init();
  display_clock.init();
  display_power.init();

  display.power = (PowerStatus*) &display_power;
  display.clock = (Clock*) &display_clock;
  display.pong = (Pong*) &display_pong;

  display.system = watch;

  runDisplayTask((Display *) &display, &display_task);

  Actor::setWatch(watch);
  Actor::setPower((PowerManager *) &power_manager);
  Actor::setDisplayTask(display_task);

  runActor("watchface", &display_clock, 9);
  runActor("pong", &display_pong, 10);
  runActor("power", &display_power, 11);

  // connectWifi();
}

void loop() {
  vTaskDelay(10000);
}
