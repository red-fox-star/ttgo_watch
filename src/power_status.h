#include "actor.h"

class PowerStatus : public Actor {
  public:
    bool ready();
    void run();
    void init();

  private:
    int last_run = 0;
};
