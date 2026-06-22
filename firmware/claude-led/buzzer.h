#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "config.h"

// ===== 蜂鸣器状态 =====
bool buzzerOn = false;
unsigned long buzzerStart = 0;
unsigned long buzzerDuration = 200;

// ===== 蜂鸣器控制 =====
// 无源蜂鸣器需要 PWM 驱动，ESP32 Core 3.x 用 ledc
#define BUZZER_FREQ  2000  // 频率 2000Hz
#define BUZZER_RESOLUTION 8 // 8 位分辨率

void buzz(unsigned long durationMs) {
  if (durationMs == 0) return;
  buzzerOn = true;
  buzzerStart = millis();
  buzzerDuration = durationMs;
  ledcWriteTone(PIN_BUZZER, BUZZER_FREQ);
}

void updateBuzz() {
  if (buzzerOn && millis() - buzzerStart >= buzzerDuration) {
    ledcWriteTone(PIN_BUZZER, 0);
    buzzerOn = false;
  }
}

#endif
