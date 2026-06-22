#ifndef COMMAND_H
#define COMMAND_H

#include <Arduino.h>
#include "config.h"
#include "buzzer.h"

// ===== 状态 =====
int currentMode = 0;
unsigned long modeStartTime = 0;  // 模式开始时间

// ===== 状态切换 =====
void setMode(int mode) {
  if (currentMode == mode) return;
  if (mode < 0) mode = 0;
  if (mode > 19) mode = 19;
  currentMode = mode;
  modeStartTime = millis();  // 记录模式开始时间
  Serial.print("Mode: ");
  Serial.println(mode);
}

// ===== 命令解析 =====
// 命令格式：mode[,buzzer_duration_ms]
//   mode: 0-19 数字或名称别名
//   buzzer_duration_ms: 蜂鸣器响的毫秒数，0=不响，默认0
//   示例："alarm,200" → 模式6 + 蜂鸣器响200ms
//         "alarm,20"  → 模式6 + 极短滴一声
//         "alarm,0"   → 模式6 + 不响
//         "alarm"     → 模式6 + 不响
bool handleCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();

  // 解析逗号分隔的参数
  String modeStr = cmd;
  String buzzerStr = "0";
  int commaIdx = cmd.indexOf(',');
  if (commaIdx >= 0) {
    modeStr = cmd.substring(0, commaIdx);
    buzzerStr = cmd.substring(commaIdx + 1);
    buzzerStr.trim();
  }
  unsigned long buzzMs = buzzerStr.toInt();

  // 先尝试数字
  int num = modeStr.toInt();
  if (num >= 0 && num <= 19 && (String(num) == modeStr || modeStr == "0")) {
    setMode(num);
    if (buzzMs > 0) buzz(buzzMs);
    return true;
  }

  // Claude Code 别名
  if (modeStr == "off" || modeStr == "close" || modeStr == "idle") { setMode(0); return true; }
  if (modeStr == "thinking" || modeStr == "think" || modeStr == "green" || modeStr == "blink") { setMode(2); return true; }
  if (modeStr == "busy" || modeStr == "executing") { setMode(7); return true; }
  if (modeStr == "success" || modeStr == "ok" || modeStr == "done") { setMode(5); return true; }
  if (modeStr == "error" || modeStr == "failed") { setMode(3); if (buzzMs > 0) buzz(buzzMs); return true; }
  if (modeStr == "alarm" || modeStr == "permission") { setMode(6); if (buzzMs > 0) buzz(buzzMs); return true; }
  if (modeStr == "taichi" || modeStr == "running" || modeStr == "run") { setMode(17); return true; }

  // 名称别名
  if (cmd == "bothoff") { setMode(0); return true; }
  if (cmd == "bothflash") { setMode(1); return true; }
  if (cmd == "greenflash") { setMode(2); return true; }
  if (cmd == "redflash") { setMode(3); return true; }
  if (cmd == "yellowflash") { setMode(4); return true; }
  if (cmd == "greenon") { setMode(5); return true; }
  if (cmd == "redon") { setMode(6); return true; }
  if (cmd == "yellowon") { setMode(7); return true; }
  if (cmd == "bothon") { setMode(8); return true; }
  if (cmd == "police") { setMode(9); return true; }
  if (cmd == "heartbeat") { setMode(10); return true; }
  if (cmd == "sos") { setMode(11); return true; }
  if (cmd == "breathing") { setMode(12); return true; }
  if (cmd == "firefly") { setMode(13); return true; }
  if (cmd == "ecg") { setMode(14); return true; }
  if (cmd == "phasechase" || cmd == "chase") { setMode(15); return true; }
  if (cmd == "strobe" || cmd == "strobechase") { setMode(16); return true; }
  if (cmd == "hello" || cmd == "hellomorse") { setMode(18); return true; }
  if (cmd == "radar") { setMode(19); return true; }

  return false;
}

#endif
