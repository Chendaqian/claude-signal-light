#ifndef MUSIC_H
#define MUSIC_H

#include <Arduino.h>
#include <pgmspace.h>
#include "config.h"

// ===== 音乐播放器状态 =====
bool musicPlaying = false;
const uint16_t *musicMelody = NULL;
uint16_t musicLen = 0;
uint16_t musicIndex = 0;
unsigned long musicNoteStart = 0;
unsigned long musicNoteMs = 0;

// ===== 8051 频率表 (Hz) =====
// 由 FREQH[]/FREQL[] 转换而来，晶振 11.0592MHz
// 索引: 0-6=低音1-7, 7-13=中音1-7, 14-20=高音2-7, 21-27=超高音1-7
static const uint16_t PROGMEM freqTable[] = {
  // octave 1 (低音)
  1571, 1763, 1979, 2099, 2351, 2639, 2963,
  // octave 2 (中音)
  3143, 3528, 3961, 4189, 4702, 5281, 5926,
  // octave 3 (高音)
  6276, 7035, 7899, 8365, 9404, 10552, 11840,
  // octave 4 (超高音)
  12538, 14070, 15798, 16756, 18808, 21105, 23630
};

// ===== 启动短音 (~2.5秒) =====
static const uint16_t PROGMEM skyStartup[] = {
  6,  180,  7,  180,  8,  540,  7,  180,  8,  360, 10,  360,  7,  720
};
static const uint16_t skyStartupLen = sizeof(skyStartup) / sizeof(skyStartup[0]) / 2;

// ===== 完整天空之城 (~46秒) =====
static const uint16_t PROGMEM skyFull[] = {
   0,  180,  0,  180,  0,  180,  6,  180,  7,  180,  8,  540,  7,  180,  8,  360,
  10,  360,  7,  720,  0,  360,  3,  180,  3,  180,  6,  540,  5,  180,  6,  360,
   8,  360,  5,  720,  0,  360,  3,  360,  4,  540,  3,  180,  4,  360,  8,  360,
   3,  540,  0,  180,  8,  180,  8,  180,  8,  180,  7,  540,  4,  180,  4,  360,
   7,  360,  7,  720,  0,  360,  6,  180,  7,  180,  8,  540,  7,  180,  8,  360,
  10,  360,  7,  720,  0,  360,  3,  180,  3,  180,  6,  540,  5,  180,  6,  360,
   8,  360,  5,  720,  0,  360,  2,  180,  3,  180,  4,  360,  8,  180,  7,  360,
   8,  540,  9,  180,  9,  180, 10,  180,  8,  540,  0,  360,  8,  180,  7,  180,
   6,  180,  6,  180,  7,  360,  5,  360,  6,  720,  0,  360,  8,  180,  9,  180,
  10,  540,  9,  180, 10,  360, 12,  360,  9,  720,  0,  360,  5,  180,  5,  180,
   8,  540,  7,  180,  8,  360, 10,  360, 10,  720,  0,  360,  0,  360,  6,  180,
   7,  180,  8,  360,  7,  360,  9,  180,  9,  180,  8,  540,  5,  180,  5,  540,
   0,  180, 11,  360, 10,  360,  9,  360,  8,  360, 10,  720,  0,  360, 10,  360,
  13,  720, 12,  360, 12,  360, 10,  180,  9,  180,  8,  720,  0,  180,  8,  180,
   9,  360,  8,  180,  9,  180,  9,  360, 12,  360, 10,  720,  0,  360, 10,  360,
  13,  720, 12,  720, 10,  180,  9,  180,  8,  720,  0,  180,  8,  180,  9,  360,
   8,  180,  9,  180,  9,  360,  7,  360,  6,  360
};
static const uint16_t skyFullLen = sizeof(skyFull) / sizeof(skyFull[0]) / 2;

// ===== 播放控制 =====

// 开始播放（非阻塞）
// full=0 播启动短音, full=1 播完整版
void startMusic(bool full = false) {
  musicMelody = full ? skyFull : skyStartup;
  musicLen = full ? skyFullLen : skyStartupLen;
  musicIndex = 0;
  musicPlaying = true;
  musicNoteStart = millis();

  // 读取第一个音符的时值
  musicNoteMs = pgm_read_word(&musicMelody[1]);

  // 播放第一个音符
  uint8_t noteIdx = pgm_read_word(&musicMelody[0]);
  if (noteIdx > 0) {
    uint16_t freq = pgm_read_word(&freqTable[noteIdx - 1]);
    ledcAttach(PIN_BUZZER, freq, 8);
    ledcWriteTone(PIN_BUZZER, freq);
  }
}

// 更新播放（每 loop 调用，非阻塞）
void updateMusic() {
  if (!musicPlaying) return;

  unsigned long now = millis();
  if (now - musicNoteStart < musicNoteMs) return;

  // 下一个音符
  musicIndex++;
  if (musicIndex >= musicLen) {
    ledcWriteTone(PIN_BUZZER, 0);
    musicPlaying = false;
    return;
  }

  musicNoteStart = now;

  uint8_t noteIdx = pgm_read_word(&musicMelody[musicIndex * 2]);
  uint16_t duration = pgm_read_word(&musicMelody[musicIndex * 2 + 1]);
  musicNoteMs = duration;

  if (noteIdx > 0) {
    uint16_t freq = pgm_read_word(&freqTable[noteIdx - 1]);
    ledcAttach(PIN_BUZZER, freq, 8);
    ledcWriteTone(PIN_BUZZER, freq);
  } else {
    ledcWriteTone(PIN_BUZZER, 0);  // 休止
  }
}

// 停止播放
void stopMusic() {
  if (!musicPlaying) return;
  ledcWriteTone(PIN_BUZZER, 0);
  musicPlaying = false;
}

#endif
