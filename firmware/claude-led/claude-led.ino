/*
  Claude-LED-LUCKEY
  ESP32C3 蓝牙 BLE 状态指示灯

  硬件连接：
    GPIO4 → 有源蜂鸣器 (+)
    GPIO5 → LED 红灯 (R)
    GPIO6 → LED 黄灯 (Y)
    GPIO7 → LED 绿灯 (G)
    GND   → LED 板 GND + 蜂鸣器 (-)

  蓝牙设备名：Claude-LED-LUCKEY

  灯效模式（0-19）：
    0  = 全灭
    1  = 三灯同频闪烁 500ms
    2  = 绿灯闪烁 500ms
    3  = 红灯闪烁 500ms
    4  = 黄灯闪烁 500ms
    5  = 绿灯常亮
    6  = 红灯常亮
    7  = 黄灯常亮
    8  = 三灯常亮
    9  = 警车交替快闪
    10 = 心跳双闪
    11 = SOS 摩尔斯求救码
    12 = 三色轮转呼吸灯
    13 = 萤火虫混沌呼吸
    14 = 心电波模拟
    15 = 三相正弦追逐
    16 = 急救爆闪追击
    17 = 太极呼吸
    18 = HELLO 摩尔斯码
    19 = 雷达扫描与锁定

  Claude Code 别名：
    off / close / idle → 0
    thinking / think   → 2
    busy / executing   → 7
    success / ok / done→ 5
    error / failed     → 3
    alarm / permission → 6
    green / blink      → 2
    taichi             → 17
*/

#include "config.h"
#include "led.h"
#include "buzzer.h"
#include "modes.h"
#include "command.h"
#include "ble.h"

// ===== LED 动画更新 =====
void updateLed() {
  unsigned long now = millis();
  if (currentMode >= 0 && currentMode <= 19) {
    modes[currentMode](now);
  }
}

// ===== 初始化 =====
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("=== Claude-LED-LUCKEY ===");
  Serial.print("Brightness R=");
  Serial.print(BRIGHTNESS_RED);
  Serial.print(" Y=");
  Serial.print(BRIGHTNESS_YELLOW);
  Serial.print(" G=");
  Serial.println(BRIGHTNESS_GREEN);

  esp_task_wdt_deinit();

  // 初始化引脚
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  setRgb(0, 0, 0);
  digitalWrite(PIN_BLUE, LOW);  // 初始蓝灯灭

  // 初始化蜂鸣器（ESP32 Core 3.x API）
  ledcAttach(PIN_BUZZER, BUZZER_FREQ_1, BUZZER_RESOLUTION);
  ledcWriteTone(PIN_BUZZER, 0);

  // 首次通电默认灯效：HELLO 摩尔斯码
  setMode(18);

  // 初始化 BLE
  setupBLE();
}

void loop() {
  updateBLE();

  // 自动关闭检查：模式 0 是全灭，不需要自动关闭
  if (currentMode != 0 && millis() - modeStartTime >= AUTO_OFF_MS) {
    Serial.println("Auto off: 5 min timeout");
    setMode(0);
  }

  updateLed();
  updateBuzz();
  delay(10);
}
