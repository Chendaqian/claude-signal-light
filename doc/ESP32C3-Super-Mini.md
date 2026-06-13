# ESP32C3 Super Mini 开发板文档

## 简介

ESP32C3 Super Mini 是一款基于 Espressif ESP32-C3 WiFi/蓝牙双模芯片的 IoT 迷你开发板。ESP32-C3 是一款 32 位 RISC-V CPU，包含 FPU（浮点单元），可进行 32 位单精度运算，具有强大的计算能力。它具有出色的射频性能，支持 IEEE 802.11b/g/n WiFi 和蓝牙 5（LE）协议。该板附带外部天线，可增强无线应用的信号强度。它还具有小巧精致的外形并结合单面表面贴装设计。它配备了丰富的接口，有 11 个可用作 PWM 引脚的数字 I/O 和 4 个可用作 ADC 引脚的模拟 I/O。它支持 UART、I2C 和 SPI 等串行接口。板上还有一个小的重置按钮和一个引导加载程序模式按钮。

综合以上特点，ESP32C3 Super Mini 定位为高性能、低功耗、高性价比的物联网迷你开发板，适用于低功耗物联网应用和无线可穿戴应用。

<center>ESP32C3-SuperMini</center>

---

## 硬件描述

### 产品参数

- **强大的 CPU**: ESP32-C3, 32 位 RISC-V 单核处理器, 运行频率高达 160 MHz
- **WiFi**: 802.11b/g/n 协议、2.4GHz、支持 Station 模式、SoftAP 模式、SoftAP+Station 模式、混杂模式
- **蓝牙**: Bluetooth 5.0
- **超低功耗**: 深度睡眠功耗约 43μA
- **丰富的板子资源**: 400KB SRAM、384KB ROM，内置 4M flash
- **芯片型号**: ESP32C3FN4
- **超小尺寸**: 小至拇指 (22.52×18mm)，适用于可穿戴设备和小型项目
- **可靠的安全功能**: 支持 AES-128/256、哈希、RSA、HMAC、数字签名和安全启动的加密硬件加速器
- **丰富的接口**: 1×I2C、1×SPI、2×UART、11×GPIO(PWM)、4×ADC
- **单面元件、表面贴装设计**
- **板载 LED 蓝灯**: GPIO8 引脚

### 引脚图（文字描述）

> 基于板子丝印及常见功能整理，所有数字 GPIO 均支持 PWM（最多 11 路同时）。

| 引脚 | 功能                                       | 备注                          |
|------|--------------------------------------------|-------------------------------|
| 3V3  | 3.3V 输出                                  | 可对外供电                    |
| EN   | 复位（低电平复位）                         | 板上有 RESET 按钮             |
| IO2  | GPIO2, ADC1_CH2, 触摸                      | 可作 PWM / ADC                |
| IO3  | GPIO3, ADC1_CH3                            | 可作 PWM / ADC                |
| IO4  | GPIO4, ADC1_CH4, 触摸                      | 可作 PWM / ADC                |
| IO5  | GPIO5, ADC2_CH0, 触摸                      | 可作 PWM / ADC                |
| IO6  | GPIO6, FSPICLK / 通用                      | 可作 PWM                      |
| IO7  | GPIO7, FSPID / 通用                        | 可作 PWM                      |
| IO8  | GPIO8                                      | **板载蓝色 LED**（低电平点亮）|
| IO9  | GPIO9                                      | 可作 PWM                      |
| IO10 | GPIO10                                     | 可作 PWM                      |
| IO18 | GPIO18, SPI_MOSI / 通用                    | 可作 PWM                      |
| IO19 | GPIO19, SPI_MISO / 通用                    | 可作 PWM                      |
| IO20 | GPIO20, SPI_SCK / 通用                     | 可作 PWM                      |
| IO21 | GPIO21, UART_RX0 / 通用                    | 可作 PWM                      |
| IO22 | GPIO22, UART_TX0 / 通用                    | 可作 PWM                      |
| 5V   | 5V 输入（3.3~6V）                          | 外部供电，不可同时用 USB      |
| GND  | 地                                         | 多个 GND 引脚                 |

**其他接口说明**：
- **I2C**：任意 GPIO 可软件定义（常用 IO4=SDA, IO5=SCL）
- **SPI**：IO18(MOSI), IO19(MISO), IO20(SCK)，片选可自定义
- **UART**：默认 UART0 为 IO21(RX), IO22(TX)；USB Type-C 也提供串口
- **ADC 通道**：共 4 路可用（IO2~IO5 等）
- **按钮**：BOOT（进入下载模式）、RESET（复位）

### 尺寸图

<center>[图片：尺寸图，见原始文档第3页]</center>

### 原理图

<center>[图片：原理图，见原始文档第4页]</center>

### 外接电源

如果需要外部供电，只需将外部电源正极接入 5V 的位置，GND 接负极（支持 3.3~6V 电源）。切记连接外部电源时，无法接入 USB，USB 和外部供电只能选择一个。

### 注意

焊接时请注意不要使正负极短路，烧坏电池和设备。

### WiFi 天线

如果想使用外置天线，可以按照图片外接外置天线。

---

## 入门

### 硬件设置

您需要准备以下内容：
- 1 个 ESP32C3 Super Mini
- 1 台电脑
- 1 根 USB Type-C 数据线

> **提示**：有些 USB 线只能供电，不能传输数据。如果您没有 USB 线或者不知道您的 USB 线是否可以传输数据，可以购买 Type-C 数据线。

**步骤 1**：通过 USB Type-C 数据线将 ESP32C3 Super Mini 连接到计算机。

---

## 软件设置

**步骤 1**：根据您的操作系统下载并安装最新版本的 Arduino IDE。  
（如果下载缓慢可以在国内 Arduino 社区下载）

**步骤 2**：启动 Arduino 应用程序。

**步骤 3**：将 ESP32 板包添加到 Arduino IDE。  
导航到 `File > Preferences`，在 "Additional Boards Manager URL" 中填写以下地址：