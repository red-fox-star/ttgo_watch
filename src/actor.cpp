#include "actor.h"

TTGOClass * Actor::watch = nullptr;
TFT_eSPI * Actor::screen = nullptr;
PowerManager * Actor::power = nullptr;
void Actor::setPower(PowerManager * _power) { power = _power; }

