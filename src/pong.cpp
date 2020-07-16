/*
 * Pong
 * Original Code from https://github.com/rparrett/pongclock
 *
 */

#include "pong.h"

unsigned short int dly = 10;

int16_t h = 240;
int16_t w = 240;

int16_t paddle_h = 25;
int16_t paddle_w = 2;

int16_t lpaddle_x = 0;
int16_t rpaddle_x = w - paddle_w;

int16_t lpaddle_y = 0;
int16_t rpaddle_y = h - paddle_h;

int16_t lpaddle_d = 1;
int16_t rpaddle_d = -1;

int16_t lpaddle_ball_t = w - w / 4;
int16_t rpaddle_ball_t = w / 4;

int16_t target_y = 0;

int16_t ball_x = 2;
int16_t ball_y = 2;
int16_t oldball_x = 2;
int16_t oldball_y = 2;

int16_t ball_dx = 1;
int16_t ball_dy = 1;

int16_t ball_w = 4;
int16_t ball_h = 4;

int16_t dashline_h = 4;
int16_t dashline_w = 2;
int16_t dashline_n = h / dashline_h;
int16_t dashline_x = w / 2 - 1;
int16_t dashline_y = dashline_h / 2;

int16_t lscore = 12;
int16_t rscore = 4;


Pong::Pong() {
  h = 240;
  w = 240;

  dly = 100;

  paddle_h = 25;
  paddle_w = 2;

  lpaddle_x = 0;
  rpaddle_x = w - paddle_w;

  lpaddle_y = 0;
  rpaddle_y = h - paddle_h;

  lpaddle_d = 1;
  rpaddle_d = -1;

  lpaddle_ball_t = w - w / 4;
  rpaddle_ball_t = w / 4;

  target_y = 0;

  ball_x = 2;
  ball_y = 2;
  oldball_x = 2;
  oldball_y = 2;

  ball_dx = 1;
  ball_dy = 1;

  ball_w = 4;
  ball_h = 4;

  dashline_h = 4;
  dashline_w = 2;
  dashline_n = h / dashline_h;
  dashline_x = w / 2 - 1;
  dashline_y = dashline_h / 2;

  lscore = 12;
  rscore = 4;
}

void Pong::init() {
  lpaddle_y = random(0, h - paddle_h);
  rpaddle_y = random(0, h - paddle_h);

  // ball is placed on the center of the left paddle
  ball_y = lpaddle_y + (paddle_h / 2);

  calc_target_y();
}

void Pong::run() {
  delay_time = dly;
  refresh_display = true;
}

void Pong::display() {
  lpaddle();
  rpaddle();
  midline();
  ball();
}

void Pong::midline() {
  // If the ball is not on the line then don't redraw the line
  if ((ball_x < dashline_x - ball_w) && (ball_x > dashline_x + dashline_w)) return;

  // Quick way to draw a dashed line
  screen->setWindow(dashline_x, 0, dashline_x + dashline_w - 1, 0 + h - 1);

  for (int16_t i = 0; i < dashline_n; i += 2) {
    screen->pushColor(TFT_WHITE, dashline_w * dashline_h); // push dash pixels
    screen->pushColor(TFT_BLACK, dashline_w * dashline_h); // push gap pixels
  }
}


void Pong::calc_target_y() {
    int16_t target_x;
    int16_t reflections;
    int16_t y;

    if (ball_dx == 1) {
        target_x = w - ball_w;
    } else {
        target_x = -1 * (w - ball_w);
    }

    y = abs(target_x * (ball_dy / ball_dx) + ball_y);

    reflections = floor(y / h);

    if (reflections % 2 == 0) {
        target_y = y % h;
    } else {
        target_y = h - (y % h);
    }
}

int Pong::speed() {
  int new_speed = dly * random(2);
  if (new_speed > 20) new_speed = 20;
  if (new_speed < 5) new_speed = 5;
  return new_speed;
}

void Pong::lpaddle() {

    if (lpaddle_d == 1) {
        screen->fillRect(lpaddle_x, lpaddle_y, paddle_w, 1, TFT_BLACK);
    } else if (lpaddle_d == -1) {
        screen->fillRect(lpaddle_x, lpaddle_y + paddle_h - 1, paddle_w, 1, TFT_BLACK);
    }

    lpaddle_y = lpaddle_y + lpaddle_d;

    if (ball_dx == 1) lpaddle_d = 0;
    else {
        if (lpaddle_y + paddle_h / 2 == target_y) lpaddle_d = 0;
        else if (lpaddle_y + paddle_h / 2 > target_y) lpaddle_d = -1;
        else lpaddle_d = 1;
    }

    if (lpaddle_y + paddle_h >= h && lpaddle_d == 1) lpaddle_d = 0;
    else if (lpaddle_y <= 0 && lpaddle_d == -1) lpaddle_d = 0;

    screen->fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, TFT_WHITE);
}

void Pong::rpaddle() {
    if (rpaddle_d == 1) {
        screen->fillRect(rpaddle_x, rpaddle_y, paddle_w, 1, TFT_BLACK);
    } else if (rpaddle_d == -1) {
        screen->fillRect(rpaddle_x, rpaddle_y + paddle_h - 1, paddle_w, 1, TFT_BLACK);
    }

    rpaddle_y = rpaddle_y + rpaddle_d;

    if (ball_dx == -1) rpaddle_d = 0;
    else {
        if (rpaddle_y + paddle_h / 2 == target_y) rpaddle_d = 0;
        else if (rpaddle_y + paddle_h / 2 > target_y) rpaddle_d = -1;
        else rpaddle_d = 1;
    }

    if (rpaddle_y + paddle_h >= h && rpaddle_d == 1) rpaddle_d = 0;
    else if (rpaddle_y <= 0 && rpaddle_d == -1) rpaddle_d = 0;

    screen->fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_WHITE);
}

void Pong::ball() {
    ball_x = ball_x + ball_dx;
    ball_y = ball_y + ball_dy;

    dly = speed();

    if (ball_dx == -1 && ball_x == paddle_w && ball_y + ball_h >= lpaddle_y && ball_y <= lpaddle_y + paddle_h) {
        ball_dx = ball_dx * -1;
        calc_target_y();
    } else if (ball_dx == 1 && ball_x + ball_w == w - paddle_w && ball_y + ball_h >= rpaddle_y && ball_y <= rpaddle_y + paddle_h) {
        ball_dx = ball_dx * -1;
        calc_target_y();
    } else if ((ball_dx == 1 && ball_x >= w) || (ball_dx == -1 && ball_x + ball_w < 0)) {
        dly = 5;
    }

    if (ball_y > h - ball_w || ball_y < 0) {
        ball_dy = ball_dy * -1;
        ball_y += ball_dy; // Keep in bounds
    }

    screen->drawRect(oldball_x, oldball_y, ball_w, ball_h, TFT_BLACK);
    screen->drawRect(   ball_x,    ball_y, ball_w, ball_h, TFT_WHITE);
    oldball_x = ball_x;
    oldball_y = ball_y;
}

