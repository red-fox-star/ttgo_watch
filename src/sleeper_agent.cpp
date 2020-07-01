#include "sleeper_agent.h"

bool SleeperAgent::low_power = false;

bool SleeperAgent::asleep() {
  return low_power;
}

bool SleeperAgent::ready() {
  return true;
}

void SleeperAgent::init() {
  powerUp();
}

void SleeperAgent::run() {
  if (low_power) {
    checkTouch();

    if (last_touch > sleep_time) {
      powerUp();
    }
  } else {
    if (timestamp() - wake_time > 5000) {
      powerDown();
    }
  }
}

bool SleeperAgent::checkTouch() {
  if (watch->touch->touched() > 0) {
    last_touch = timestamp();
    return true;
  }

  return false;
}

void SleeperAgent::powerDown() {
  // setCpuFrequencyMhz(40);
  watch->closeBL();
  watch->displaySleep();

  low_power = true;
  sleep_time = timestamp();
}

void SleeperAgent::powerUp() {
  // setCpuFrequencyMhz(240);
  // watch->bl->adjust(255);
  watch->displayWakeup();
  watch->openBL();

  wake_time = timestamp();
  low_power = false;
}

