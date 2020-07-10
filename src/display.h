#pragma once

#include "power_status.h"
#include "clock.h"
#include "serial_message_queue.h"

#define DISPLAY_POWER 0x1
#define DISPLAY_PONG 0x2
#define DISPLAY_WATCH 0x4

class Display {
  public:
    PowerStatus * power;
    Clock * watch;
    Pong * pong;

    void run() {
      if (update_key & power->displayIdentifier()) {
        power->display();
        power->displayPerformed();
      }

      if (update_key & watch->displayIdentifier()) {
        watch->display();
        watch->displayPerformed();
      }

      if (update_key & pong->displayIdentifier()) {
        pong->display();
        pong->displayPerformed();
      }
    }

    void notified_by(uint32_t notification_value) {
      update_key = notification_value;
    }

  private:
    uint32_t update_key = 0;
};
