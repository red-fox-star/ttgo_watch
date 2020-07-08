#define LILYGO_TWATCH_2020_V1

#include <TTGO.h>
#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "pong.h"
#include "clock.h"
#include "power_status.h"
#include "display.h"

TTGOClass *watch;
PCF8563_Class *rtc;

Pong pong;
Clock watchface;
PowerStatus power;
Display display;
TaskHandle_t display_task;

static portMUX_TYPE display_mutex;

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
      Serial.println("WiFi connected");
      ntpRead();
  }, WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);

  WiFi.enableSTA(true);
  WiFi.begin();
}

void buildTask(const char * name, Actor * object, int priority = 2) {
  xTaskCreate(
      [](void* object){
        for (;;) {
          bool update_display = true;
          unsigned int delay_time = 100;

          Actor* actor = (Actor *) object;
          Displayable* displayable = (Displayable *) object;

          actor->execute(delay_time, update_display);

          if (update_display) {
            auto yolo = xTaskNotify(
              display_task,
              displayable->displayIdentifier(),
              eSetValueWithoutOverwrite
            );

            if (yolo != pdPASS) {
              Serial.printf("couldn't notify for display of  %#x\n", displayable->displayIdentifier());
            }
          }

          // Serial.printf("delaying %#x by %i\n", displayable->displayIdentifier(), delay_time);
          vTaskDelay(delay_time / portTICK_PERIOD_MS);
        }
        Serial.println("deleting a task");
        vTaskDelete(NULL);
      },
      name, 10000, (void *) object, priority, NULL
  );
}

void setupDisplayTask() {
  display.power = (PowerStatus*) &power;
  display.watchface = (Clock*) &watchface;
  display.pong = (Pong*) &pong;

  vPortCPUInitializeMutex(&display_mutex);

  xTaskCreate(
    [](void* object) {
      Display* display = (Display *) object;
      BaseType_t received;
      uint32_t notification_value;

      for (;;) {
        received = xTaskNotifyWait(0xffffffff, 0xffffffff, &notification_value, 10000);
        if (received != pdTRUE) continue;
        // Serial.printf("notified by %#x\n", notification_value);

        portENTER_CRITICAL(&display_mutex);
        display->notified_by(notification_value);
        display->run();
        portEXIT_CRITICAL(&display_mutex);
      }

      Serial.println("deleting the display task!");
      vTaskDelete(NULL);
    },
    "display", 10000, (void *) &display, 30, &display_task
 );
}

void setup(void) {
  // setCpuFrequencyMhz(80);
  // disableCore1WDT();
  // disableCore0WDT();

  Serial.begin(115200);

  randomSeed(analogRead(0)*analogRead(1));

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

  pinMode(AXP202_INT, INPUT);
  attachInterrupt(AXP202_INT, [] {
      power.interrupt();
  }, FALLING);

  WiFi.mode(WIFI_OFF);

  pong.init();
  watchface.init();
  power.init();

  delay(10);
  setupDisplayTask();

  // buildTask("pong", &pong, 10);
  buildTask("watchface", &watchface, 9);
  // buildTask("power", &power, 11);

  // connectWifi();

  delay(5000);
}

int last = 0;

void touchin() {
}

void loop() {
  vTaskDelay(10000);
}
