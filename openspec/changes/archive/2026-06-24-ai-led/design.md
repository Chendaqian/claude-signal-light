## Context

用户需要一个硬件提醒系统，当 Claude Code 执行任务时，通过 LED 灯和蜂鸣器提供物理状态提醒。当前参考项目使用 WiFi/TCP 方案，但用户希望使用蓝牙 BLE 方案，避免网络配置复杂性。

硬件清单：
- ESP32C3-Super-Mini 开发板（支持蓝牙 5.0 LE）
- 三色 LED 电路板（红/黄/绿，GND/R/Y/G 引脚）
- 有源蜂鸣器

接线定义（基于店家文档）：
| 组件 | 引脚 | ESP32C3 GPIO | 板子位置 |
|------|------|--------------|---------|
| LED 红 | R | GPIO5 | 右侧第1个 |
| LED 黄 | Y | GPIO6 | 右侧第2个 |
| LED 绿 | G | GPIO7 | 右侧第3个 |
| LED 地 | GND | GND | 左侧第2个 |
| 蜂鸣器正 | + | GPIO4 | 左侧第4个 |
| 蜂鸣器负 | - | GND | 左侧第2个 |

注意：GPIO8 是板载蓝灯，GPIO0-GPIO3 保留给 ADC/SPI

## Goals / Non-Goals

**Goals:**
- 通过蓝牙 BLE 无线连接 ESP32 和 Windows 电脑
- 支持 Claude Code 所有关键 hook 事件的状态提醒
- LED 灯效直观区分不同状态（思考中、执行中、成功、错误、需要权限）
- 蜂鸣器在需要权限或出错时发声提醒

**Non-Goals:**
- 不支持多设备同时连接
- 不实现复杂的灯效动画（保持简洁直观）
- 不支持 WiFi 远程控制（仅蓝牙近场）

## Decisions

### 1. 通信协议：蓝牙 BLE vs WiFi TCP

**选择：蓝牙 BLE**

理由：
- 无需 WiFi 网络配置，即插即用
- 延迟更低（本地通信）
- 参考项目使用 WiFi，但用户明确要求蓝牙

替代方案：WiFi TCP（参考项目方案）- 需要配置网络，不够便携

### 2. PC 端通信方案

**选择：PowerShell + .NET BLE 库**

理由：
- Windows 原生支持，无需额外安装
- 可直接在 Claude Code hooks 中调用
- 使用 `Windows.Devices.Bluetooth` WinRT API

替代方案：Python + bleak 库 - 需要安装 Python 环境

### 3. 状态映射方案

**选择：简化状态集**

理由：
- 参考项目有 7 种状态，但用户硬件只有 3 色 LED
- 映射为 5 种核心状态：thinking（绿灯闪烁）、busy（黄灯常亮）、success（绿灯常亮）、error（红灯闪烁）、alarm（红灯常亮+蜂鸣）

### 4. 蜂鸣器控制策略

**选择：仅在 alarm（权限请求）时鸣响**

理由：
- 蜂鸣器仅用于提醒用户需要确认权限，避免频繁打扰
- 有源蜂鸣器只需高低电平控制
- 鸣响时长 200ms，不重复

## Risks / Trade-offs

- **蓝牙连接稳定性** → 实现自动重连机制，超时后 LED 显示断连状态
- **Windows BLE 驱动兼容性** → 要求 Windows 10 1803+，文档说明
- **Hook 超时限制** → PowerShell 脚本需在 2 秒内完成，蓝牙连接需预建立
- **LED 引脚冲突** → ESP32C3 的 GPIO8 是板载蓝灯，避免使用
