#pragma once
#include "actor.h"

class PowerStatus : public Actor {
  public:
    bool ready();
    void run();

    void display();
    const uint32_t displayIdentifier() {
      return 0x1;
    }

  private:
    long unsigned int last_run = 0;
    char display_buffer[255];

    void pluggedInSummary();
    void batterySummary();
};
