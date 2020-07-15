#pragma once

#include "moving_average.h"

class PowerManager {
  public:
    void init();
    void interrupt();
    void run();

    bool pluggedIn();
    bool charging();
    bool lowPower();
    int batteryPercentage();
    float batteryCurrent();
    float vbusCurrent();

    void powerDown();
    void powerUp();
    void sleepHarder();

  private:
    static TTGOClass *watch;
    static TFT_eSPI *screen;

    // state of the system
    bool _plugged_in = false;
    bool _charging = false;
    bool _read_irq = false;

    // statistics about the system
    int battery_percentage;
    MovingAverage<float, 100> battery_current;
    MovingAverage<float, 100> vbus_current;
    long unsigned int last_logged = 0;
    void logPower();

    // managing the sleep/low_power/high_power state
    long unsigned int last_touch = 0;
    long unsigned int wake_time = 0;
    long unsigned int sleep_time = 0;
    long unsigned int last_interaction = 0;

    bool low_power = false;
    bool should_wake = false;

    void readIRQ();
    void charging(bool state);
    void pluggedIn(bool state);
    bool checkTouch();

    void sleepOrWake();
    void tryToWake();
    void tryToSleep();
    void tryToSleepHarder();
};

void powerManagementTask(void* object);
void runPowerManagement(PowerManager * power_manager);
