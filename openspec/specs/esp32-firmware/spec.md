## ADDED Requirements

### Requirement: 硬件接线定义
ESP32C3-Super-Mini 与外部硬件的连接引脚定义如下。

#### Scenario: LED 电路板连接
- **WHEN** 连接三色 LED 电路板（共阴极，高电平点亮）
- **THEN** GND 接 ESP32 GND（左侧第2个引脚），R(红) 接 GPIO5（右侧第1个），Y(黄) 接 GPIO6（右侧第2个），G(绿) 接 GPIO7（右侧第3个）

#### Scenario: 无源蜂鸣器连接
- **WHEN** 连接无源蜂鸣器模块（3线制：VCC、IO、GND）
- **THEN** VCC 接 3V3（左侧第3个引脚），IO 接 GPIO4（左侧第4个引脚），GND 接 ESP32 GND（左侧第2个引脚）

#### Scenario: 避免引脚冲突
- **WHEN** 选择 GPIO 引脚
- **THEN** 不使用 GPIO8（板载蓝灯，右侧第4个引脚）

#### Scenario: LED 亮度配置
- **WHEN** 固件中定义 LED 最大亮度
- **THEN** 使用常量 BRIGHTNESS_RED（255）、BRIGHTNESS_YELLOW（255）、BRIGHTNESS_GREEN（192），所有灯效函数引用这些常量

### Requirement: 蓝牙 BLE 服务
ESP32C3 SHALL 启动 BLE GATT 服务，提供可写特征值用于接收状态命令。服务 UUID 为自定义 128 位 UUID，特征值支持 Write Without Response。

#### Scenario: 蓝牙广播
- **WHEN** ESP32 启动完成
- **THEN** 设备开始 BLE 广播，设备名为 "Claude-LED-LUCKEY"

#### Scenario: 接收状态命令
- **WHEN** 通过 BLE 写入 UTF-8 字符串命令（格式为 `mode[,buzzer_param]`）
- **THEN** ESP32 解析命令并切换对应 LED 状态，buzzer_param 参数控制蜂鸣器模式

#### Scenario: 命令格式
- **WHEN** 接收命令字符串
- **THEN** 命令格式为 `mode[,buzzer_param]`，其中 mode 为 0-19 数字或名称，buzzer_param 为蜂鸣器参数（0=不响，1=短响一声，2=短响两声，3=高频短响，默认 0）

### Requirement: LED 灯效控制
ESP32 SHALL 根据状态命令控制三色 LED（红/黄/绿）显示 20 种不同灯效。

#### Scenario: 模式 0 - 全灭
- **WHEN** 接收 "0" 或 "off" 命令
- **THEN** 所有 LED 熄灭

#### Scenario: 模式 1 - 三灯同频闪烁
- **WHEN** 接收 "1" 或 "bothflash" 命令
- **THEN** 红黄绿同时闪烁，500ms 亮 / 500ms 灭

#### Scenario: 模式 2 - 绿灯闪烁
- **WHEN** 接收 "2"、"thinking" 或 "greenflash" 命令
- **THEN** 绿灯闪烁，500ms 周期

#### Scenario: 模式 3 - 红灯闪烁
- **WHEN** 接收 "3"、"error" 或 "redflash" 命令
- **THEN** 红灯闪烁，500ms 周期

#### Scenario: 模式 4 - 黄灯闪烁
- **WHEN** 接收 "4" 或 "yellowflash" 命令
- **THEN** 黄灯闪烁，500ms 周期

#### Scenario: 模式 5 - 绿灯常亮
- **WHEN** 接收 "5"、"success" 或 "greenon" 命令
- **THEN** 绿灯持续点亮

#### Scenario: 模式 6 - 红灯常亮
- **WHEN** 接收 "6"、"alarm" 或 "redon" 命令
- **THEN** 红灯持续点亮

#### Scenario: 模式 7 - 黄灯常亮
- **WHEN** 接收 "7"、"busy" 或 "yellowon" 命令
- **THEN** 黄灯持续点亮

#### Scenario: 模式 8 - 三灯常亮
- **WHEN** 接收 "8" 或 "bothon" 命令
- **THEN** 红黄绿同时点亮

#### Scenario: 模式 9 - 警车交替快闪
- **WHEN** 接收 "9" 或 "police" 命令
- **THEN** 红绿交替 300ms + 黄灯独立快闪 150ms

#### Scenario: 模式 10 - 心跳双闪
- **WHEN** 接收 "10" 或 "heartbeat" 命令
- **THEN** 80ms 亮 → 100ms 灭 → 80ms 亮 → 600ms 灭

#### Scenario: 模式 11 - SOS 摩尔斯求救码
- **WHEN** 接收 "11" 或 "sos" 命令
- **THEN** 三短三长三短循环

#### Scenario: 模式 12 - 三色轮转呼吸灯
- **WHEN** 接收 "12" 或 "breathing" 命令
- **THEN** 红绿反向渐变 + 黄灯 120° 相位偏移

#### Scenario: 模式 13 - 萤火虫混沌呼吸
- **WHEN** 接收 "13" 或 "firefly" 命令
- **THEN** 三灯独立正弦波，周期不同产生非对称律动

#### Scenario: 模式 14 - 心电波模拟
- **WHEN** 接收 "14" 或 "ecg" 命令
- **THEN** 红灯 ECG 波形，绿灯 QRS 峰暴闪，黄灯 P/T 波柔和发光

#### Scenario: 模式 15 - 三相正弦追逐
- **WHEN** 接收 "15" 或 "phasechase" 命令
- **THEN** 绿 sin、红 cos、黄 sin+120°，3s 周期

#### Scenario: 模式 16 - 急救爆闪追击
- **WHEN** 接收 "16" 或 "strobechase" 命令
- **THEN** 绿 → 黄 → 红各闪 3 下，80ms 亮/80ms 灭

#### Scenario: 模式 17 - 太极呼吸
- **WHEN** 接收 "17" 或 "taichi" 命令
- **THEN** 红绿 sin³ 对立消长，黄灯独立 120° 相位偏移

#### Scenario: 模式 18 - HELLO 摩尔斯码
- **WHEN** 接收 "18" 或 "hello" 命令
- **THEN** 持续广播 HELLO 摩尔斯码

#### Scenario: 模式 19 - 雷达扫描与锁定
- **WHEN** 接收 "19" 或 "radar" 命令
- **THEN** 3s 绿灯正弦扫描 + 黄灯回波 → 1s 红灯锁定闪动 → 0.5s 三灯全亮

### Requirement: 首次通电默认灯效
ESP32 SHALL 在首次通电时自动设置 LED 灯效为 HELLO 摩尔斯码（模式 18）。

#### Scenario: 首次通电
- **WHEN** ESP32 上电启动
- **THEN** 自动设置灯效为模式 18（HELLO 摩尔斯码）

#### Scenario: 自动关闭
- **WHEN** ESP32 运行超过 5 分钟无操作
- **THEN** 自动关闭所有 LED（模式 0）

### Requirement: 蜂鸣器控制
ESP32 SHALL 根据命令中的 buzzer_param 参数控制无源蜂鸣器，支持 3 种模式。

#### Scenario: 蜂鸣器不响
- **WHEN** 接收命令格式为 `mode` 或 `mode,0`（如 "alarm" 或 "alarm,0"）
- **THEN** 蜂鸣器不鸣响

#### Scenario: 蜂鸣器短响一声
- **WHEN** 接收命令格式为 `mode,1`（如 "alarm,1"）
- **THEN** 蜂鸣器鸣响 80ms（主板自检音，频率 2000Hz）

#### Scenario: 蜂鸣器短响两声
- **WHEN** 接收命令格式为 `mode,2`（如 "alarm,2"）
- **THEN** 蜂鸣器鸣响两声，第一声 2000Hz，第二声 1000Hz（不同声调），每声 80ms，间隔 60ms

#### Scenario: 蜂鸣器高频短响
- **WHEN** 接收命令格式为 `mode,3`（如 "alarm,3"）
- **THEN** 蜂鸣器鸣响 200ms，频率 4000Hz

### Requirement: 天空之城彩蛋
ESP32 SHALL 支持通过命令播放天空之城旋律，包含启动短音和完整版两种模式。

#### Scenario: 启动音效
- **WHEN** ESP32 上电启动
- **THEN** 自动播放天空之城前奏短音（~2.5秒），非阻塞，LED 动画不受影响

#### Scenario: 播放短版
- **WHEN** 接收 "castle" 或 "sky" 命令
- **THEN** 播放天空之城前奏短音（~2.5秒）

#### Scenario: 播放完整版
- **WHEN** 接收 "castle,1" 或 "sky,1" 命令
- **THEN** 播放天空之城完整旋律（~46秒）

#### Scenario: 停止播放
- **WHEN** 播放中再次接收 "castle" 或 "sky" 命令
- **THEN** 停止当前播放

### Requirement: 串口调试输出
ESP32 SHALL 通过串口输出当前状态变化信息，波特率 115200。

#### Scenario: 状态变化日志
- **WHEN** 状态发生变化
- **THEN** 串口输出 "Mode: <模式编号>"

#### Scenario: 接收命令日志
- **WHEN** 接收 BLE 命令
- **THEN** 串口输出 "RX: <命令内容>"
