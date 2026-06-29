#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "config.h"

// ===== 蜂鸣器状态 =====
bool buzzerOn = false;
unsigned long buzzerStart = 0;
unsigned short buzzBeepMs = 80;   // 每拍时长（ms）

// 多响支持（模式2用：两声不同调）
int buzzRemaining = 0;       // 剩余响的次数
int buzzPhase = 0;           // 0=响第一声, 1=间隔, 2=响第二声
unsigned long buzzPhaseStart = 0;

// 通用多响支持（模式4、5、7用）
byte buzzBeepsLeft = 0;      // 剩余拍数
unsigned short buzzGapMs = 60;    // 拍间隔（ms）
int buzzOnFreq = 2000;            // 响的频率（Hz）

// ===== 蜂鸣器配置 =====
// 无源蜂鸣器需要 PWM 驱动，ESP32 Core 3.x 用 ledc
#define BUZZER_RESOLUTION 8       // 8 位分辨率
#define BUZZER_FREQ_1 2000        // 第一声频率 2000Hz
#define BUZZER_FREQ_2 1000        // 第二声频率 1000Hz（不同声调）
#define BUZZER_FREQ_3 4000        // 第三声频率 4000Hz（高频提示音）
#define BUZZER_BEEP_MS 80         // 每声时长 80ms
#define BUZZER_BEEP3_MS 200       // 第三声时长 200ms
#define BUZZER_GAP_MS 60          // 两声之间间隔 60ms

// 蜂鸣器参数：
//   0 = 不响
//   1 = 短响一声（80ms，主板自检音）
//   2 = 短响两声（两次不同声调）
//   3 = 高频短响（200ms，4000Hz）
//   4 = 三连短响（80ms × 3，间隔60ms）
//   5 = 一长一短（300ms + 80ms）
//   6 = 长响（500ms）
//   7 = 急促四连（50ms × 4，间隔40ms）

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
    // 短响一声（80ms，2000Hz）
    buzzerOn = true;
    buzzerStart = millis();
    buzzBeepMs = BUZZER_BEEP_MS;
    buzzBeepsLeft = 0;
    startBuzz(BUZZER_FREQ_1);
  } else if (buzzParam == 2) {
    // 短响两声（2000Hz+1000Hz，不同声调）
    buzzerOn = true;
    buzzRemaining = 2;
    buzzPhase = 0;
    buzzPhaseStart = millis();
    buzzerStart = millis();
    buzzBeepMs = BUZZER_BEEP_MS;
    startBuzz(BUZZER_FREQ_1);
  } else if (buzzParam == 3) {
    // 高频短响（200ms，4000Hz）
    buzzerOn = true;
    buzzerStart = millis();
    buzzBeepMs = BUZZER_BEEP3_MS;
    buzzBeepsLeft = 0;
    startBuzz(BUZZER_FREQ_3);
  } else if (buzzParam == 4) {
    // 三连短响：80ms × 3，间隔 60ms
    buzzBeepsLeft = 3;
    buzzBeepMs = 80;
    buzzGapMs = 60;
    buzzOnFreq = 2000;
    buzzPhase = 0;
    buzzPhaseStart = millis();
    buzzerStart = millis();
    startBuzz(buzzOnFreq);
  } else if (buzzParam == 5) {
    // 一长一短：300ms + 80ms
    buzzBeepsLeft = 2;
    buzzBeepMs = 300;
    buzzGapMs = 60;
    buzzOnFreq = 2000;
    buzzPhase = 0;
    buzzPhaseStart = millis();
    buzzerStart = millis();
    startBuzz(buzzOnFreq);
  } else if (buzzParam == 6) {
    // 长响：500ms
    buzzerOn = true;
    buzzerStart = millis();
    buzzBeepMs = 500;
    buzzBeepsLeft = 0;
    startBuzz(BUZZER_FREQ_1);
  } else if (buzzParam == 7) {
    // 急促四连：50ms × 4，间隔 40ms
    buzzBeepsLeft = 4;
    buzzBeepMs = 50;
    buzzGapMs = 40;
    buzzOnFreq = 2000;
    buzzPhase = 0;
    buzzPhaseStart = millis();
    buzzerStart = millis();
    startBuzz(buzzOnFreq);
  }
}

void updateBuzz() {
  if (!buzzerOn && buzzRemaining <= 0 && buzzBeepsLeft <= 0) return;

  unsigned long now = millis();

  // ===== 模式2专用：两声不同调（buzzRemaining > 0）=====
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
          buzzerOn = false;
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

  // ===== 通用多响逻辑（模式4、5、7）=====
  if (buzzBeepsLeft > 0) {
    if (buzzPhase == 0) {
      if (now - buzzerStart >= buzzBeepMs) {
        stopBuzz();
        buzzBeepsLeft--;
        if (buzzBeepsLeft > 0) {
          buzzPhase = 1;  // 进入间隔
          buzzPhaseStart = now;
          // 模式5：第一拍300ms后，最后一拍切到80ms
          if (buzzBeepsLeft == 1 && buzzBeepMs == 300) {
            buzzBeepMs = 80;
          }
        } else {
          buzzerOn = false;
        }
      }
    } else if (buzzPhase == 1) {
      // 间隔，等待后响下一声
      if (now - buzzPhaseStart >= buzzGapMs) {
        buzzPhase = 0;
        buzzerStart = now;
        startBuzz(buzzOnFreq);
      }
    }
    return;
  }

  // ===== 单次响（模式1、3、6）=====
  if (buzzerOn && now - buzzerStart >= buzzBeepMs) {
    stopBuzz();
    buzzerOn = false;
  }
}

#endif
