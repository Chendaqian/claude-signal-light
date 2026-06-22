#ifndef MODES_H
#define MODES_H

#include <Arduino.h>
#include <math.h>
#include "config.h"
#include "led.h"

// ===== 灯效模式函数 =====

// 模式 0: 全灭
void mode0_off(unsigned long now) {
  setRgb(0, 0, 0);
}

// 模式 1: 三灯同频闪烁 500ms
void mode1_bothFlash(unsigned long now) {
  bool on = (now / 500) % 2 == 0;
  setRgb(on ? BRIGHTNESS_RED : 0, on ? BRIGHTNESS_YELLOW : 0, on ? BRIGHTNESS_GREEN : 0);
}

// 模式 2: 绿灯闪烁 500ms
void mode2_greenFlash(unsigned long now) {
  bool on = (now / 500) % 2 == 0;
  setRgb(0, 0, on ? BRIGHTNESS_GREEN : 0);
}

// 模式 3: 红灯闪烁 500ms
void mode3_redFlash(unsigned long now) {
  bool on = (now / 500) % 2 == 0;
  setRgb(on ? BRIGHTNESS_RED : 0, 0, 0);
}

// 模式 4: 黄灯闪烁 500ms
void mode4_yellowFlash(unsigned long now) {
  bool on = (now / 500) % 2 == 0;
  setRgb(0, on ? BRIGHTNESS_YELLOW : 0, 0);
}

// 模式 5: 绿灯常亮
void mode5_greenOn(unsigned long now) {
  setRgb(0, 0, BRIGHTNESS_GREEN);
}

// 模式 6: 红灯常亮
void mode6_redOn(unsigned long now) {
  setRgb(BRIGHTNESS_RED, 0, 0);
}

// 模式 7: 黄灯常亮
void mode7_yellowOn(unsigned long now) {
  setRgb(0, BRIGHTNESS_YELLOW, 0);
}

// 模式 8: 三灯常亮
void mode8_bothOn(unsigned long now) {
  setRgb(BRIGHTNESS_RED, BRIGHTNESS_YELLOW, BRIGHTNESS_GREEN);
}

// 模式 9: 警车交替快闪，红绿灯反相交替，黄灯独立闪烁 150ms
void mode9_police(unsigned long now) {
  bool phase = (now / 300) % 2 == 0;
  bool yellowFlash = (now / 150) % 2 == 0;
  uint8_t r = phase ? 0 : BRIGHTNESS_RED;
  uint8_t g = phase ? BRIGHTNESS_GREEN : 0;
  uint8_t y = yellowFlash ? BRIGHTNESS_YELLOW : 0;
  setRgb(r, y, g);
}

// 模式 10: 心跳双闪，80ms亮→100ms灭→80ms亮→600ms灭
void mode10_heartbeat(unsigned long now) {
  const int pattern[] = {80, 100, 80, 600};
  const int patternLen = 4;
  long cycleMs = 0;
  for (int i = 0; i < patternLen; i++) cycleMs += pattern[i];
  long pos = now % cycleMs;
  int step = 0;
  long accumulated = 0;
  for (int i = 0; i < patternLen; i++) {
    accumulated += pattern[i];
    if (pos < accumulated) { step = i; break; }
  }
  bool on = step % 2 == 0;
  setRgb(on ? BRIGHTNESS_RED : 0, on ? BRIGHTNESS_YELLOW : 0, on ? BRIGHTNESS_GREEN : 0);
}

// 模式 11: SOS 摩尔斯求救码
void mode11_sos(unsigned long now) {
  const int pattern[] = {
    200, 200, 200, 200, 200, 200,
    400,
    600, 200, 600, 200, 600, 200,
    400,
    200, 200, 200, 200, 200, 1000
  };
  const int patternLen = 20;
  long cycleMs = 0;
  for (int i = 0; i < patternLen; i++) cycleMs += pattern[i];
  long pos = now % cycleMs;
  int step = 0;
  long accumulated = 0;
  for (int i = 0; i < patternLen; i++) {
    accumulated += pattern[i];
    if (pos < accumulated) { step = i; break; }
  }
  bool on = step % 2 == 0;
  setRgb(on ? BRIGHTNESS_RED : 0, on ? BRIGHTNESS_YELLOW : 0, on ? BRIGHTNESS_GREEN : 0);
}

// 模式 12: 三色轮转呼吸灯，红绿反向渐变 + 黄灯 120° 相位偏移
void mode12_breathing(unsigned long now) {
  const double cycle = 4092.0;
  double t = (now % (long)cycle) / cycle;
  double val = t < 0.5 ? t * 2 : 2 - (t * 2);
  uint8_t g = (uint8_t)(val * BRIGHTNESS_GREEN);
  uint8_t r = (uint8_t)((1 - val) * BRIGHTNESS_RED);

  double t2 = ((now + 1364) % (long)cycle) / cycle;
  double val2 = t2 < 0.5 ? t2 * 2 : 2 - (t2 * 2);
  uint8_t y = (uint8_t)(val2 * BRIGHTNESS_YELLOW);

  setRgb(r, y, g);
}

// 模式 13: 萤火虫混沌呼吸，三灯独立正弦波
void mode13_firefly(unsigned long now) {
  double valGreen = (sin((now % 3000) * 2.0 * PI / 3000.0) + 1.0) * 0.5;
  double valRed = (sin((now % 2200) * 2.0 * PI / 2200.0) + 1.0) * 0.5;
  double valYellow = (sin((now % 2600) * 2.0 * PI / 2600.0) + 1.0) * 0.5;

  setRgb(
    (uint8_t)(valRed * BRIGHTNESS_RED),
    (uint8_t)(valYellow * BRIGHTNESS_YELLOW),
    (uint8_t)(valGreen * BRIGHTNESS_GREEN)
  );
}

// 模式 14: 心电波模拟
void mode14_ecg(unsigned long now) {
  long pos = now % 1200;
  double rVal, gVal, yVal;

  if (pos < 100) {
    rVal = pos / 100.0 * 0.15;
  } else if (pos < 200) {
    rVal = 0;
  } else if (pos < 280) {
    rVal = 1.0;
  } else if (pos < 450) {
    rVal = 0;
  } else if (pos < 700) {
    double t = (pos - 450) / 250.0;
    rVal = sin(t * PI) * 0.3;
  } else {
    rVal = 0;
  }

  gVal = (pos >= 200 && pos < 280) ? 1.0 : 0;

  if (pos < 100) {
    yVal = pos / 100.0 * 0.4;
  } else if (pos < 200) {
    yVal = 0;
  } else if (pos < 280) {
    yVal = 0.6;
  } else if (pos < 450) {
    yVal = 0;
  } else if (pos < 700) {
    double yt = (pos - 450) / 250.0;
    yVal = sin(yt * PI) * 0.5;
  } else {
    yVal = 0;
  }

  setRgb(
    (uint8_t)(rVal * BRIGHTNESS_RED),
    (uint8_t)(yVal * BRIGHTNESS_YELLOW),
    (uint8_t)(gVal * BRIGHTNESS_GREEN)
  );
}

// 模式 15: 三相正弦追逐，绿 sin、红 cos、黄 sin+120°，3s 周期
void mode15_phaseChase(unsigned long now) {
  double t = (now % 3000) * 2.0 * PI / 3000.0;
  double valGreen = (sin(t) + 1.0) * 0.5;
  double valRed = (cos(t) + 1.0) * 0.5;
  double valYellow = (sin(t + (2.0 * PI / 3.0)) + 1.0) * 0.5;

  setRgb(
    (uint8_t)(valRed * BRIGHTNESS_RED),
    (uint8_t)(valYellow * BRIGHTNESS_YELLOW),
    (uint8_t)(valGreen * BRIGHTNESS_GREEN)
  );
}

// 模式 16: 急救爆闪追击，绿→黄→红各闪3下，80ms亮/80ms灭
void mode16_strobeChase(unsigned long now) {
  long pos = now % 1680;

  if (pos < 480) {
    bool on = (int)(pos % 160) < 80;
    setRgb(0, 0, on ? BRIGHTNESS_GREEN : 0);
  } else if (pos < 960) {
    bool on = (int)((pos - 480) % 160) < 80;
    setRgb(0, on ? BRIGHTNESS_YELLOW : 0, 0);
  } else if (pos < 1440) {
    bool on = (int)((pos - 960) % 160) < 80;
    setRgb(on ? BRIGHTNESS_RED : 0, 0, 0);
  } else {
    setRgb(0, 0, 0);
  }
}

// 模式 17: 太极呼吸，红绿对立消长，黄灯独立
void mode17_taichi(unsigned long now) {
  double t = (now % 3000) * 2.0 * PI / 3000.0;
  double s = sin(t);
  double s3 = s * s * s;
  double valGreen = (s3 + 1.0) * 0.5;
  double valRed = (1.0 - s3) * 0.5;
  // 黄灯独立 120° 相位偏移
  double tYellow = ((now + 1000) % 3000) * 2.0 * PI / 3000.0;
  double valYellow = (sin(tYellow) + 1.0) * 0.5;

  setRgb(
    (uint8_t)(valRed * BRIGHTNESS_RED),
    (uint8_t)(valYellow * BRIGHTNESS_YELLOW),
    (uint8_t)(valGreen * BRIGHTNESS_GREEN)
  );
}

// 模式 18: HELLO 摩尔斯码
void mode18_helloMorse(unsigned long now) {
  // H: ....  E: .  L: .-..  L: .-..  O: ---
  // 每个字符: 符号时长 + 200ms间隔，最后去掉末尾间隔，加上字母间隔
  const int pattern[] = {
    // H: . . . .
    200, 200, 200, 200, 200, 200, 200, 600,
    // E: .
    200, 600,
    // L: . - . .
    200, 200, 600, 200, 200, 200, 200, 600,
    // L: . - . .
    200, 200, 600, 200, 200, 200, 200, 600,
    // O: - - -
    600, 200, 600, 200, 600, 1400
  };
  const int patternLen = 32;
  long cycleMs = 0;
  for (int i = 0; i < patternLen; i++) cycleMs += pattern[i];
  long pos = now % cycleMs;
  int step = 0;
  long accumulated = 0;
  for (int i = 0; i < patternLen; i++) {
    accumulated += pattern[i];
    if (pos < accumulated) { step = i; break; }
  }
  bool on = step % 2 == 0;
  setRgb(on ? BRIGHTNESS_RED : 0, on ? BRIGHTNESS_YELLOW : 0, on ? BRIGHTNESS_GREEN : 0);
}

// 模式 19: 雷达扫描与锁定
void mode19_radar(unsigned long now) {
  long pos = now % 4500;

  if (pos < 3000) {
    // 扫描阶段：绿灯正弦扫描 + 黄灯回波
    double t = pos * 2.0 * PI / 3000.0;
    double val = (200.0 / 1023.0) + ((823.0 / 1023.0) * (sin(t) + 1.0) * 0.5);
    double ping = (sin(t) + 1.0) * 0.5;
    double yVal = ping > 0.85 ? (ping - 0.85) / 0.15 : 0;
    setRgb(
      30,
      (uint8_t)(yVal * BRIGHTNESS_YELLOW),
      (uint8_t)(val * BRIGHTNESS_GREEN)
    );
  } else if (pos < 4000) {
    // 锁定闪动
    bool flash = ((int)(pos - 3000) / 250) % 2 == 0;
    if (flash) {
      setRgb(BRIGHTNESS_RED, 0, BRIGHTNESS_GREEN);
    } else {
      setRgb(0, BRIGHTNESS_YELLOW, 0);
    }
  } else {
    // 全亮
    setRgb(BRIGHTNESS_RED, BRIGHTNESS_YELLOW, BRIGHTNESS_GREEN);
  }
}

// ===== LED 动画调度 =====
typedef void (*ModeFunc)(unsigned long);
ModeFunc modes[] = {
  mode0_off,
  mode1_bothFlash,
  mode2_greenFlash,
  mode3_redFlash,
  mode4_yellowFlash,
  mode5_greenOn,
  mode6_redOn,
  mode7_yellowOn,
  mode8_bothOn,
  mode9_police,
  mode10_heartbeat,
  mode11_sos,
  mode12_breathing,
  mode13_firefly,
  mode14_ecg,
  mode15_phaseChase,
  mode16_strobeChase,
  mode17_taichi,
  mode18_helloMorse,
  mode19_radar
};

#endif
