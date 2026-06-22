# Claude-LED-LUCKEY 🚦

<div align="center">
  <img width="300" height="236" style="display: block; margin: 0 auto;" alt="aiLED" src="https://github.com/user-attachments/assets/ff76e92f-a932-449c-ae15-31b27944fd02" />
</div>

[English](README.md)

[![Arduino](https://img.shields.io/badge/Arduino-ESP32C3-00979D)](https://www.arduino.cc/)
[![BLE](https://img.shields.io/badge/BLE-Bluetooth%205.0-blue)](https://www.bluetooth.com/)
[![Python](https://img.shields.io/badge/Python-3.8+-3776AB)](https://www.python.org/)
[![Windows](https://img.shields.io/badge/Platform-Windows-0078d4)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/ChenDaqian/claude-signal-light?style=flat)](https://github.com/ChenDaqian/claude-signal-light/stargazers)

ESP32C3 蓝牙 BLE 状态指示灯，用于 Claude Code 工作状态的物理提醒。

## 硬件清单

| 组件 | 型号/规格 |
|------|----------|
| 开发板 | ESP32C3-Super-Mini |
| LED 电路板 | 三色 LED（共阴极，红/黄/绿） |
| 蜂鸣器 | 有源蜂鸣器（低电平触发） |

## 接线定义

| 组件 | 引脚 | ESP32C3 GPIO | 板子位置 |
|------|------|--------------|----------|
| LED 红 | R | GPIO5 | 右侧第 1 个 |
| LED 黄 | Y | GPIO6 | 右侧第 2 个 |
| LED 绿 | G | GPIO7 | 右侧第 3 个 |
| LED 地 | GND | GND | 左侧第 2 个 |
| 蜂鸣器正 | + | GPIO4 | 左侧第 4 个 |
| 蜂鸣器负 | - | GND | 左侧第 2 个 |

> 注意：GPIO8 是板载蓝灯，GPIO0-GPIO3 保留给 ADC/SPI，不要使用。

<img width="2784" height="1854" alt="image" src="https://github.com/user-attachments/assets/b9610df2-42ad-4027-97fe-0090b5382298" />

## LED 亮度配置

固件中定义了三种颜色的最大亮度常量（0-255），可按需调整：

```cpp
#define BRIGHTNESS_RED    255
#define BRIGHTNESS_YELLOW 255
#define BRIGHTNESS_GREEN  192
```

修改后重新烧录固件即可全局生效。

## 灯效模式（共 20 种）

| 模式 | 名称 | 说明 |
|------|------|------|
| 0 | 全灭 | 所有 LED 熄灭 |
| 1 | 三灯同频闪烁 | 红黄绿同时闪烁，500ms 周期 |
| 2 | 绿灯闪烁 | 绿灯闪烁，500ms 周期 |
| 3 | 红灯闪烁 | 红灯闪烁，500ms 周期 |
| 4 | 黄灯闪烁 | 黄灯闪烁，500ms 周期 |
| 5 | 绿灯常亮 | 绿灯持续点亮 |
| 6 | 红灯常亮 | 红灯持续点亮 |
| 7 | 黄灯常亮 | 黄灯持续点亮 |
| 8 | 三灯常亮 | 红黄绿同时点亮 |
| 9 | 警车交替快闪 | 红绿交替 300ms + 黄灯独立快闪 150ms |
| 10 | 心跳双闪 | 80ms 亮 → 100ms 灭 → 80ms 亮 → 600ms 灭 |
| 11 | SOS 摩尔斯求救码 | 三短三长三短 |
| 12 | 三色轮转呼吸灯 | 红绿反向渐变 + 黄灯 120° 相位偏移 |
| 13 | 萤火虫混沌呼吸 | 三灯独立正弦波，周期不同产生非对称律动 |
| 14 | 心电波模拟 | 红灯 ECG 波形，绿灯 QRS 峰暴闪，黄灯 P/T 波柔和发光 |
| 15 | 三相正弦追逐 | 绿 sin、红 cos、黄 sin+120°，3s 周期 |
| 16 | 急救爆闪追击 | 绿 → 黄 → 红各闪 3 下，80ms 亮/80ms 灭 |
| 17 | 太极呼吸 | 红绿 sin³ 对立消长，黄灯独立 120° 相位偏移 |
| 18 | HELLO 摩尔斯码 | 持续广播 HELLO 摩尔斯码 |
| 19 | 雷达扫描与锁定 | 3s 绿灯正弦扫描 + 黄灯回波 → 1s 红灯锁定闪动 → 0.5s 三灯全亮 |

## 蓝牙通信

- 设备名：`Claude-LED-LUCKEY`
- 服务 UUID：`12345678-1234-1234-1234-123456789abc`
- 特征值 UUID：`12345678-1234-1234-1234-123456789abd`
- 写入方式：Write Without Response
- 数据格式：UTF-8 字符串

### 支持的命令

#### 命令格式

```
mode[,buzzer_duration_ms]
```

- `mode`：模式编号（0-19）或名称
- `buzzer_duration_ms`：蜂鸣器响的毫秒数，`0`=不响，默认 `0`

示例：
- `"6,200"` → 模式 6（红灯常亮）+ 蜂鸣器响 200ms
- `"alarm,20"` → 红灯常亮 + 极短滴一声
- `"alarm,0"` → 红灯常亮 + 不响
- `"alarm"` → 红灯常亮 + 不响
- `"2"` → 模式 2（绿灯闪烁），蜂鸣器不响

#### 数字命令（0-19）

直接发送模式编号，如 `"0"`、`"17"`。

#### 名称命令

| 命令 | 等效模式 |
|------|----------|
| off / close / idle | 0 |
| thinking / think | 2 |
| busy / executing | 7 |
| success / ok / done | 5 |
| error / failed | 3 |
| alarm / permission | 6 |
| green / blink | 2 |
| taichi / running / run | 17 |
| bothoff | 0 |
| bothflash | 1 |
| greenflash | 2 |
| redflash | 3 |
| yellowflash | 4 |
| greenon | 5 |
| redon | 6 |
| yellowon | 7 |
| bothon | 8 |
| police | 9 |
| heartbeat | 10 |
| sos | 11 |
| breathing | 12 |
| firefly | 13 |
| ecg | 14 |
| phasechase / chase | 15 |
| strobe / strobechase | 16 |
| hello / hellomorse | 18 |
| radar | 19 |

## 使用方法

### 1. 烧录固件

用 Arduino IDE 打开 `firmware/claude-led/claude-led.ino`，选择 ESP32C3 开发板，烧录。

### 2. 测试灯效

```bash
# 自动测试全部灯效（每种 3 秒）
python scripts/test-all-effects.py

# 手动测试单个灯效
python scripts/send-hook.py 17        # 太极呼吸
python scripts/send-hook.py taichi    # 同上
python scripts/send-hook.py 6 200     # 红灯常亮 + 蜂鸣器响 200ms
python scripts/send-hook.py alarm 20  # 红灯常亮 + 极短滴一声
```

### 3. Claude Code 集成

在 `~/.claude/settings.json` 中添加 hooks：

```json
{
  "hooks": {
    "SessionStart": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\path\\to\\send-hook.py\" 17",
            "timeout": 5000
          }
        ]
      }
    ],
    "PreToolUse": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\path\\to\\send-hook.py\" 2",
            "timeout": 5000
          }
        ]
      }
    ],
    "PermissionRequest": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\path\\to\\send-hook.py\" 4 200",
            "timeout": 5000
          }
        ]
      }
    ]
  }
}
```

### Claude Code 事件与灯效映射

| Hook 事件 | 参数 | 灯效 | 蜂鸣器 |
|-----------|------|------|--------|
| SessionStart | 17 | 太极呼吸 | 不响 |
| UserPromptSubmit | 2 | 绿灯闪烁 | 不响 |
| PreToolUse | 2 | 绿灯闪烁 | 不响 |
| PostToolUseFailure | 3 | 红灯闪烁 | 不响 |
| PermissionRequest | 4 200 | 黄灯闪烁 | 响 200ms |
| Stop | 5 | 绿灯常亮 | 不响 |
| SessionEnd | 17 | 太极呼吸 | 不响 |

## 依赖

- Arduino IDE + ESP32 开发板支持包
- Python 3.8+
- bleak（Python BLE 库）：`pip install bleak`

## Contributors

<a href="https://github.com/Chendaqian/claude-signal-light/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Chendaqian/claude-signal-light" />
</a>

## Star History

<a href="https://www.star-history.com/?repos=Chendaqian%2Fclaude-signal-light&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/chart?repos=Chendaqian/claude-signal-light&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/chart?repos=Chendaqian/claude-signal-light&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/chart?repos=Chendaqian/claude-signal-light&type=date&legend=top-left" />
 </picture>
</a>
