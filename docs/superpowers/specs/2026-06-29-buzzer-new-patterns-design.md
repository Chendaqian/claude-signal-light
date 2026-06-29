# 蜂鸣器新增节奏模式

## 概述

为 ESP32 Claude-LED 灯箱的蜂鸣器增加 4 种新的响铃模式，纯节奏扩展，不绑定业务语义。

## 现有模式（不变）

| 参数 | 频率 | 时长 | 效果 |
|------|------|------|------|
| 0 | - | - | 不响 |
| 1 | 2000Hz | 80ms × 1 | 短响一声 |
| 2 | 2000Hz + 1000Hz | 各 80ms | 短响两声（不同音调） |
| 3 | 4000Hz | 200ms | 高频短响 |

## 新增模式

| 参数 | 名称 | 频率 | 节奏 | 总时长 |
|------|------|------|------|--------|
| 4 | 三连短响 | 2000Hz | ▮80ms—60ms—▮80ms—60ms—▮80ms | ~360ms |
| 5 | 一长一短 | 2000Hz | ▮300ms—60ms—▮80ms | ~440ms |
| 6 | 长响 | 2000Hz | ▮500ms | 500ms |
| 7 | 急促四连 | 2000Hz | ▮50ms—40ms—▮50ms—40ms—▮50ms—40ms—▮50ms | ~320ms |

## 实现方案

改造 `buzzer.h` 中的多响机制，目前 `buzz()` + `updateBuzz()` 硬编码支持 param 2 的两声模式，需要泛化为通用拍子机：

### 新增状态变量

```cpp
// 通用多响模式
byte buzzPatternBeeps;    // 总拍数
byte buzzPatternCount;    // 已完成拍数
unsigned short buzzPatternBeepMs;  // 每拍时长
unsigned short buzzPatternGapMs;   // 拍间隔
```

其中模式 5（一长一短）两拍时长不同，通过两阶段的 beepMs 实现：
- 第 1 拍: beepMs = 300ms
- 间隔: gapMs = 60ms  
- 第 2 拍: beepMs = 80ms

### 状态机

```
IDLE → BEEP(启动PWM) → 计时到 → stopPWM → GAP(等待间隔) → 计时到 → BEEP → ... → 所有拍完成 → IDLE
```

### 改动范围

只改 `firmware/claude-led/buzzer.h` 一个文件。

## 向后兼容

- 新参数从 4 开始，不影响现有 0-3 的行为
- `handleCommand()` 无需修改，`buzz(buzzParam)` 入口不变
