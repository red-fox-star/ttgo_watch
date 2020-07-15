
#include "display.h"

void displayTask(void* object) {
  Display* display = (Display *) object;
  uint32_t notification_value;

  for (;;) {
    auto success = xTaskNotifyWait(0xffffffff, 0xffffffff, &notification_value, 10000);
    if (success != pdPASS) continue;

    display->notified_by(notification_value);

    display->system->eTFT->startWrite();
    display->run();
    display->system->eTFT->endWrite();
  }

  q_message_ln("deleting the display task!");
  vTaskDelete(NULL);
}

void runDisplayTask(Display * display, TaskHandle_t * display_task) {
  xTaskCreate(displayTask, "display", 10000, (void *) display, 30, display_task);
}
