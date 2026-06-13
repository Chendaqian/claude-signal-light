## Why

Claude Code 在执行任务时，用户需要时刻关注终端输出来了解状态（思考中、执行中、需要权限、出错等）。当 Claude 在后台运行或用户暂时离开时，容易错过重要的状态变化（如权限请求、执行失败）。通过硬件 LED 灯和蜂鸣器提供物理提醒，用户可以在不看屏幕的情况下感知 Claude 的工作状态。

## What Changes

- 新增 ESP32C3 固件，通过蓝牙 BLE 接收状态命令，控制三色 LED（红/黄/绿）和蜂鸣器
- 新增 PowerShell 桥接脚本，接收 Claude Code hook 事件并通过蓝牙发送状态到 ESP32
- 配置 Claude Code hooks（SessionStart、PreToolUse、PostToolUse、Stop 等事件）调用 PowerShell 脚本

## Capabilities

### New Capabilities

- `esp32-firmware`: ESP32C3 蓝牙 BLE 固件，接收状态命令并控制 LED 灯效和蜂鸣器
- `powershell-bridge`: PowerShell 桥接脚本，将 Claude Code hook 事件转换为蓝牙 BLE 命令

### Modified Capabilities

（无）

## Impact

- 硬件：ESP32C3-Super-Mini 开发板 + 三色 LED 电路板 + 有源蜂鸣器
- 软件：Arduino IDE（编译固件）、PowerShell 5.1+（桥接脚本）、Claude Code settings.json（hooks 配置）
- 依赖：Windows 蓝牙 BLE 支持、.NET Framework（PowerShell BLE 通信）
