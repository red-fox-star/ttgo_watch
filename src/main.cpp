/*
 * Pong
 * Original Code from https://github.com/rparrett/pongclock
 *
 */

#define LILYGO_TWATCH_2020_V1
#define BLACK TFT_BLACK
#define WHITE TFT_WHITE
#define GREY  TFT_LIGHTGREY

// axp202 pins
// LDO2 - backlight
// LDO3 -
// LDO4 -
// exten - "external module"
// DCDC2 -
//
// 40mHz crystal clock
// valid processor frequencies:
//  240, 160, 80, 40, 20, 10

#include <TTGO.h>
#include <WiFi.h>

#include "pong.h"
#include "clock.h"
#include "power_status.h"

TTGOClass *watch;
PCF8563_Class *rtc;

Pong pong;
Clock watchface;
PowerStatus power;

bool low_power = false;

void ntpRead() {
  configTzTime("MST7MDT,M3.2.0/02:00:00,M11.1.0/02:00:00", "pool.ntp.org");

  static struct tm timeinfo;
  getLocalTime(&timeinfo);

  Serial.printf("New Clock Settings: %04d.%02d.%02d - %02d:%02d:%02d \n",
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec
  );

  watch->rtc->setDateTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec
  );
}


void setup(void) {
  // setCpuFrequencyMhz(80);

  Serial.begin(115200);

  randomSeed(analogRead(0)*analogRead(1));

  Actor::timestamp(millis());

  watch = TTGOClass::getWatch();
  watch->begin();

  Actor::setWatch(watch);

  watch->rtc->check();

  watch->bl->adjust(150);

  watch->power->setPowerOutPut(
      AXP202_EXTEN
      | AXP202_DCDC2
      | AXP202_LDO3
      | AXP202_LDO4
  , AXP202_OFF);

  watch->eTFT->setTextColor(WHITE, BLACK);
  watch->eTFT->setTextFont(8);
  watch->openBL();

  pong.init();
  watchface.init();
  power.init();

  pinMode(AXP202_INT, INPUT);
  attachInterrupt(AXP202_INT, [] {
      power.interrupt();
  }, FALLING);

  WiFi.mode(WIFI_OFF);

  // WiFi.enableSTA(true);
  // WiFi.begin();
  // ntpRead();
}

int last = 0;

void touchin() {
}

void loop() {
  Actor::timestamp(millis());

  watch->eTFT->startWrite();

  if (! PowerStatus::asleep()) {
    pong.execute();
    watchface.execute();
  }

  power.execute();

  watch->eTFT->endWrite();
}
