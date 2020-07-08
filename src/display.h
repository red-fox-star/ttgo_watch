#pragma once
#include "actor.h"

#include "power_status.h"
#include "clock.h"

#define DISPLAY_POWER 0x1
#define DISPLAY_PONG 0x2
#define DISPLAY_WATCH 0x4

class Display : public Actor {
  public:
    PowerStatus * power;
    Clock * watchface;
    Pong * pong;

    void run() {
      Serial.println("time to display things!");
      watch->eTFT->startWrite();

      if (update_key & power->displayIdentifier())
        power->display();

      if (update_key & watchface->displayIdentifier())
        watchface->display();

      if (update_key & pong->displayIdentifier())
        pong->display();

      watch->eTFT->endWrite();
      Serial.println("finished displaying things");
    }

    void notified_by(uint32_t notification_value) {
      update_key = notification_value;
    }

  private:
    uint32_t update_key = 0;
};
