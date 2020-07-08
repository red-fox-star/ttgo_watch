/*
 * Pong
 * Original Code from https://github.com/rparrett/pongclock
 *
 */
#pragma once

#define BLACK TFT_BLACK
#define WHITE TFT_WHITE
#define GREY  TFT_LIGHTGREY

#include "actor.h"
#include "displayable.h"

class Pong : public Actor, public Displayable {
  public:
    Pong();
    bool ready();
    void run();
    void init();
    void display();

    const uint32_t displayIdentifier() {
      return 0x2;
    }

  private:
    int last_run = 0;

    int16_t h;
    int16_t w;

    int dly;

    int16_t paddle_h;
    int16_t paddle_w;

    int16_t lpaddle_x;
    int16_t rpaddle_x;

    int16_t lpaddle_y;
    int16_t rpaddle_y;

    int16_t lpaddle_d;
    int16_t rpaddle_d;

    int16_t lpaddle_ball_t;
    int16_t rpaddle_ball_t;

    int16_t target_y;

    int16_t ball_x;
    int16_t ball_y;
    int16_t oldball_x;
    int16_t oldball_y;

    int16_t ball_dx;
    int16_t ball_dy;

    int16_t ball_w;
    int16_t ball_h;

    int16_t dashline_h;
    int16_t dashline_w;
    int16_t dashline_n;
    int16_t dashline_x;
    int16_t dashline_y;

    int16_t lscore;
    int16_t rscore;

    void midline();
    void calc_target_y();
    int speed();
    void lpaddle();
    void rpaddle();
    void ball();
};
