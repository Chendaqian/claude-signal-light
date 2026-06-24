#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "config.h"

// ===== 蜂鸣器状态 =====
bool buzzerOn = false;
unsigned long buzzerStart = 0;
unsigned long buzzerDuration = 0;

// 多响支持
int buzzRemaining = 0;       // 剩余响的次数
int buzzPhase = 0;           // 0=响第一声, 1=响第二声
unsigned long buzzPhaseStart = 0;

// ===== 蜂鸣器配置 =====
// 无源蜂鸣器需要 PWM 驱动，ESP32 Core 3.x 用 ledc
#define BUZZER_RESOLUTION 8       // 8 位分辨率
#define BUZZER_FREQ_1 2000        // 第一声频率 2000Hz
#define BUZZER_FREQ_2 1000        // 第二声频率 1000Hz（不同声调）
#define BUZZER_BEEP_MS 80         // 每声时长 80ms
#define BUZZER_GAP_MS 60          // 两声之间间隔 60ms

// 蜂鸣器参数：
//   0 = 不响
//   1 = 短响一声（80ms，主板自检音）
//   2 = 短响两声（两次不同声调）

// 开始响蜂鸣器
void startBuzz(int freq) {
  ledcAttach(PIN_BUZZER, freq, BUZZER_RESOLUTION);
  ledcWriteTone(PIN_BUZZER, freq);
}

// 停止蜂鸣器
void stopBuzz() {
  ledcWriteTone(PIN_BUZZER, 0);
}

void buzz(int buzzParam) {
  if (buzzParam == 0) return;

  if (buzzParam == 1) {
    // 短响一声
    buzzerOn = true;
    buzzerStart = millis();
    buzzerDuration = BUZZER_BEEP_MS;
    buzzRemaining = 0;
    startBuzz(BUZZER_FREQ_1);
  } else if (buzzParam == 2) {
    // 短响两声（两次不同声调）
    buzzerOn = true;
    buzzRemaining = 2;  // 总共要响2声
    buzzPhase = 0;      // 0=响第一声, 1=间隔, 2=响第二声
    buzzPhaseStart = millis();
    buzzerStart = millis();
    buzzerDuration = BUZZER_BEEP_MS;
    startBuzz(BUZZER_FREQ_1);
  }
}

void updateBuzz() {
  if (!buzzerOn && buzzRemaining <= 0) return;

  unsigned long now = millis();

  // 多响模式（buzzRemaining > 0）
  if (buzzRemaining > 0) {
    if (buzzPhase == 0) {
      // 当前是响第1声，检查是否该停
      if (now - buzzerStart >= BUZZER_BEEP_MS) {
        stopBuzz();
        buzzRemaining--;
        if (buzzRemaining > 0) {
          buzzPhase = 1;  // 进入间隔期
          buzzPhaseStart = now;
        } else {
          buzzerOn = false;  // 全部响完
        }
      }
    } else if (buzzPhase == 1) {
      // 间隔期，检查是否该响第2声
      if (now - buzzPhaseStart >= BUZZER_GAP_MS) {
        startBuzz(BUZZER_FREQ_2);  // 第二声用不同频率
        buzzPhase = 2;
        buzzerStart = now;
      }
    } else if (buzzPhase == 2) {
      // 响第2声，检查是否该停
      if (now - buzzerStart >= BUZZER_BEEP_MS) {
        stopBuzz();
        buzzRemaining--;
        buzzerOn = false;
      }
    }
    return;
  }

  // 单次响模式
  if (buzzerOn && now - buzzerStart >= buzzerDuration) {
    stopBuzz();
    buzzerOn = false;
  }
}

#endif
