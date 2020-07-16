#include "actor.h"

// global scope
void actorTask(void * object) {
  for (;;) {
    bool update_display = false;
    unsigned int delay_time = 0;

    Actor* actor = (Actor *) object;

    if (actor->power->lowPower() && ! actor->runDuringLowPower()) {
      vTaskDelay(actor->delayDuringLowPower() / portTICK_PERIOD_MS);
      continue;
    }

    actor->checkWakeTime();
    actor->execute(delay_time, update_display);

    if (update_display) {
      xTaskNotify(
        actor->display_task,
        actor->displayIdentifier(),
        eSetValueWithOverwrite
      );
    }
    vTaskDelay(delay_time / portTICK_PERIOD_MS);
  }
  q_message_ln("deleting a task");
  vTaskDelete(NULL);
}

void runActor(const char * name, Actor * object, int priority = 2) {
  xTaskCreate(actorTask, name, 10000, (void *) object, priority, NULL);
}

// static scope
TTGOClass * Actor::watch = nullptr;
TFT_eSPI * Actor::screen = nullptr;
PowerManager * Actor::power = nullptr;
TaskHandle_t Actor::display_task = nullptr;
unsigned int Actor::woke_up_at = 1;

void Actor::setPower(PowerManager * _power) { power = _power; }
void Actor::setDisplayTask(TaskHandle_t _display_task) { display_task = _display_task; }
void Actor::systemWokeUp() { woke_up_at = millis(); }
void Actor::setWatch(TTGOClass * _watch) {
  watch = _watch;
  screen = watch->eTFT;
}

// instance scope
void Actor::execute(unsigned int & sleep_time, bool & display_update) {
  refresh_display = false;

  if (needsInit()) setup();

  run();

  sleep_time = delay_time;
  display_update = refresh_display;
  wake_up_run = false;
}

void Actor::checkWakeTime() {
  if (last_wake_up_processed != woke_up_at) {
    last_wake_up_processed = woke_up_at;
    wake_up_run = true;
  }
}

bool Actor::wakeUpRun() {
  return wake_up_run;
}

bool Actor::needsInit() { return ! inited; }
void Actor::setup() {
  inited = true;
  init();
}
