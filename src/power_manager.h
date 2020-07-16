#pragma once

#include "moving_average.h"

class Actor;

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
    long unsigned int lastInteraction() { return last_interaction; }
    long unsigned int now() { return _now; }

    void powerDown();
    void powerUp();
    void suspend();

  private:
    uint32_t power_down_delay = 11000;
    uint32_t suspend_delay = 15000;

    static TTGOClass *watch;
    static TFT_eSPI *screen;

    // state of the system
    bool _plugged_in = false;
    bool _charging = false;
    bool _read_irq = false;
    long unsigned int _now = 0;

    // statistics about the system
    int battery_percentage;
    MovingAverage<float, 50> battery_current;
    MovingAverage<float, 50> vbus_current;
    long unsigned int last_logged = 0;
    void logPower();

    // managing the sleep/low_power/high_power state
    long unsigned int last_touch = 0;
    long unsigned int wake_time = 0;
    long unsigned int sleep_time = 0;
    long unsigned int last_interaction = 0;

    bool _low_power = false;

    void readIRQ();
    void charging(bool state);
    void pluggedIn(bool state);
    bool checkTouch();

    void sleepOrWake();
    void tryToPowerUp();
    void tryToPowerDown();
    void tryToSuspend();
};

void powerManagementTask(void* object);
void runPowerManagement(PowerManager * power_manager);
