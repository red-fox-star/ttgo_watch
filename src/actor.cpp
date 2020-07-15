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

    actor->execute(delay_time, update_display);

    if (actor->canRequestDisplay() && update_display) {
      actor->displayRequested();
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

void Actor::setPower(PowerManager * _power) { power = _power; }
void Actor::setDisplayTask(TaskHandle_t _display_task) { display_task = _display_task; }


// instance scope
void Actor::execute(unsigned int & sleep_time, bool & display_update) {
  refresh_display = false;

  if (needsInit()) setup();

  run();

  sleep_time = delay_time;
  display_update = refresh_display;
}

