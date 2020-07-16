#pragma once
#include <exception>

#include <TTGO.h>

#include "serial_message_queue.h"
#include "power_manager.h"

class Actor {
  public:
    static TTGOClass *watch;
    static TFT_eSPI *screen;
    static PowerManager * power;
    static TaskHandle_t display_task;

    // system accessors
    static void setPower(PowerManager * _power);
    static void setDisplayTask(TaskHandle_t _display_task);
    static void setWatch(TTGOClass * _watch);

    // setup and init
    bool needsInit();
    void setup();
    virtual void init() { }

    // run and display
    virtual void run() = 0;
    void execute(unsigned int & sleep_time, bool & display_update);
    virtual void display() = 0;
    virtual const uint32_t displayIdentifier() = 0;

    // low power considerations
    virtual const bool runDuringLowPower() { return false; }
    virtual const uint32_t delayDuringLowPower() { return 1000; }

    // sleep-wake
    static void systemWokeUp();
    void checkWakeTime();
    bool wakeUpRun();

  protected:
    unsigned int delay_time = 100;
    bool refresh_display = false;

  private:
    static unsigned int woke_up_at;
    unsigned int last_wake_up_processed = 0;
    bool wake_up_run = true;
    bool inited = false;
};

void actorTask(void * object);
void runActor(const char * name, Actor * object, int priority);

