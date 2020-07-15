#pragma once

// original: https://foxpost.xyz/pages/ttgo_watch/examples/queued_serial
// updated: 2020-07-10

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Arduino.h"

struct SerialMessage {
  char text[255];
  uint32_t time;
};

void q_message(String text);
void q_message(const char * text);
void q_message_ln(const char * text);
void q_message_fmt(const char * format, ...);
void runSerialMessenger();

extern TaskHandle_t serial;
extern QueueHandle_t serial_queue;
