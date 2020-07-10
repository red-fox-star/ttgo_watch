#pragma once
#include <exception>

#include <TTGO.h>

class Actor {
  public:
    bool canRequestDisplay() {
      return ! display_pending;
    }

    void displayRequested() {
      display_pending = true;
    }

    void displayPerformed() {
      display_pending = false;
    }

    static void setWatch(TTGOClass * _watch) {
      watch = _watch;
      screen = watch->eTFT;
    }

    virtual void run() = 0;
    void execute(unsigned int & sleep_time, bool & display_update) {
      refresh_display = false;

      run();

      sleep_time = delay_time;
      display_update = refresh_display;
    }

    virtual void display() = 0;
    virtual const uint32_t displayIdentifier() = 0;

  protected:
    bool display_pending = false;
    static TTGOClass *watch;
    static TFT_eSPI *screen;
    unsigned int delay_time = 100;
    bool refresh_display = false;
};
