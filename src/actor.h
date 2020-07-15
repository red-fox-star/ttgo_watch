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

    static void setWatch(TTGOClass * _watch) {
      watch = _watch;
      screen = watch->eTFT;
    }

    static void setPower(PowerManager * _power);
    static void setDisplayTask(TaskHandle_t _display_task);

    bool canRequestDisplay() { return ! display_pending; }
    void displayRequested() { display_pending = true; }
    void displayPerformed() { display_pending = false; }

    bool needsInit() { return ! inited; }
    void setup() {
      inited = true;
      init();
    }
    virtual void init() { }

    virtual void run() = 0;
    void execute(unsigned int & sleep_time, bool & display_update);

    virtual void display() = 0;
    virtual const uint32_t displayIdentifier() = 0;

    virtual const bool runDuringLowPower() { return false; }
    virtual const uint32_t delayDuringLowPower() { return 1000; }

    virtual void beforeSleep() { }
    virtual void afterSleep() { }

  protected:
    bool display_pending = false;
    bool inited = false;

    unsigned int delay_time = 100;
    bool refresh_display = false;
};

void actorTask(void * object);
void runActor(const char * name, Actor * object, int priority);

