#pragma once
#include "actor.h"
#include "serial_message_queue.h"

#include "moving_average.h"

class PowerStatus : public Actor {
  public:
    bool ready();
    void run();
    void init();
    void interrupt();

    bool pluggedIn();
    bool charging();

    void powerDown();
    void powerUp();

    void display();
    const uint32_t displayIdentifier() {
      return 0x1;
    }

    static bool asleep();


  private:

    // calculating and displaying the runtime statistics
    long unsigned int last_run = 0;
    bool _plugged_in = false;
    bool _charging = false;
    bool _read_irq = false;
    MovingAverage<float, 100> battery_current;
    MovingAverage<float, 100> vbus_current;
    long unsigned int last_logged = 0;
    char display_buffer[255];

    void logPower();
    void pluggedInSummary();
    void batterySummary();

    // managing the low_power/high_power state
    long unsigned int last_touch = 0;
    long unsigned int wake_time = 0;
    long unsigned int sleep_time = 0;
    static bool low_power;
    bool should_wake = false;

    void readIRQ();
    void charging(bool state);
    void pluggedIn(bool state);
    bool checkTouch();
    void touchWake();
};
