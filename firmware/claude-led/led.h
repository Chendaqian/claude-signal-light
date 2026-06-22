#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include "config.h"

// ===== LED 控制 =====
void setRgb(uint8_t r, uint8_t y, uint8_t g) {
  analogWrite(PIN_RED, r);
  analogWrite(PIN_YELLOW, y);
  analogWrite(PIN_GREEN, g);
}

#endif
