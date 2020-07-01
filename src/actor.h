#pragma once
#include <exception>

#include <TTGO.h>

class Actor {
  public:
    static void timestamp(int t) { _timestamp = t; }
    static int timestamp() { return _timestamp; }
    static void setWatch(TTGOClass * _watch) {
      watch = _watch;
      screen = watch->eTFT;
    }

    virtual bool ready() = 0;
    virtual void run() = 0;
    void execute(bool force = false) {
      if (ready() || force)
        run();
    }

  protected:
    static int _timestamp;
    static TTGOClass *watch;
    static TFT_eSPI *screen;
};
