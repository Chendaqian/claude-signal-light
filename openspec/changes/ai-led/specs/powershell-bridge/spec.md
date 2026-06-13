## ADDED Requirements

### Requirement: 蓝牙 BLE 连接
PowerShell/Python 脚本 SHALL 使用 BLE API 连接名为 "Claude-LED-LUCKEY" 的 ESP32 设备。

#### Scenario: 首次连接
- **WHEN** 脚本执行且无已保存设备
- **THEN** 扫描 BLE 设备，找到 "Claude-LED-LUCKEY" 并连接

#### Scenario: 连接超时
- **WHEN** 5 秒内未找到设备
- **THEN** 脚本静默退出，不阻塞 Claude Code

### Requirement: 状态命令发送
脚本 SHALL 接收命令行参数指定的状态，并通过 BLE 发送到 ESP32。

#### Scenario: 命令格式
- **WHEN** 执行脚本
- **THEN** 命令格式为 `mode[,buzzer_duration_ms]`，其中 mode 为模式编号或名称，buzzer_duration_ms 为蜂鸣器响的毫秒数（0=不响，默认 0）

#### Scenario: 发送模式命令
- **WHEN** 执行 `send-hook.py 5` 或 `send-hook.py success`
- **THEN** 通过 BLE 发送 "success" 字符串

#### Scenario: 发送带蜂鸣器的命令
- **WHEN** 执行 `send-hook.py 4 200` 或 `send-hook.py alarm 200`
- **THEN** 通过 BLE 发送 "alarm,200" 字符串，蜂鸣器响 200ms

#### Scenario: 发送极短提示音
- **WHEN** 执行 `send-hook.py 4 20` 或 `send-hook.py alarm 20`
- **THEN** 通过 BLE 发送 "alarm,20" 字符串，蜂鸣器响 20ms

#### Scenario: 不发送蜂鸣器参数
- **WHEN** 执行 `send-hook.py 4 0` 或 `send-hook.py 4`
- **THEN** 通过 BLE 发送 "alarm" 字符串，蜂鸣器不响

#### Scenario: Claude Code 兼容映射
- **WHEN** 执行 `send-hook.py 4`（Claude Code 旧编号）
- **THEN** 通过 BLE 发送 "alarm" 字符串

#### Scenario: 直接模式编号
- **WHEN** 执行 `send-hook.py 17`
- **THEN** 通过 BLE 发送 "17" 字符串

### Requirement: Claude Code Hook 集成
脚本 SHALL 支持在 Claude Code hooks 中调用，参数格式兼容现有配置。

#### Scenario: SessionStart 事件
- **WHEN** Claude Code 会话开始
- **THEN** 调用脚本发送 "17"（太极呼吸）

#### Scenario: UserPromptSubmit 事件
- **WHEN** 用户提交提示
- **THEN** 调用脚本发送 "2"（绿灯闪烁）

#### Scenario: PreToolUse 事件
- **WHEN** Claude Code 准备使用工具
- **THEN** 调用脚本发送 "2"（绿灯闪烁）

#### Scenario: PostToolUseFailure 事件
- **WHEN** 工具执行失败
- **THEN** 调用脚本发送 "3"（黄灯常亮）

#### Scenario: PermissionRequest 事件
- **WHEN** Claude Code 需要权限确认
- **THEN** 调用脚本发送 "4 200"（红灯常亮 + 蜂鸣器响 200ms）

#### Scenario: Stop 事件
- **WHEN** Claude Code 任务完成
- **THEN** 调用脚本发送 "5"（绿灯常亮）

#### Scenario: SessionEnd 事件
- **WHEN** Claude Code 会话结束
- **THEN** 调用脚本发送 "17"（太极呼吸）

### Requirement: 超时控制
脚本 SHALL 在 5 秒内完成执行，避免阻塞 Claude Code。

#### Scenario: 正常执行
- **WHEN** BLE 连接正常
- **THEN** 脚本在 1 秒内完成

#### Scenario: 连接失败
- **WHEN** BLE 连接失败
- **THEN** 脚本在 5 秒内超时退出

### Requirement: PowerShell 兼容映射
Python 脚本 SHALL 兼容 PowerShell send-hook.ps1 的 -LedMode 参数。

#### Scenario: LedMode 映射
- **WHEN** 接收 -LedMode 参数值
- **THEN** 1=off, 2=thinking, 3=busy, 4=alarm, 5=success, 6=error, 17=taichi
