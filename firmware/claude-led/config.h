#ifndef CONFIG_H
#define CONFIG_H

// ===== BLE 配置 =====
#define DEVICE_NAME "Claude-LED-LUCKEY"
//#define SERVICE_UUID "1460d279-efe7-4066-a466-e80dd498c317"
//#define CHAR_UUID "a9680f41-1111-4b02-b787-48592ff38d3b"
#define SERVICE_UUID "f4b7e3a1-5c6d-4e8f-9a2b-1c3d5e7f9a0b"
#define CHAR_UUID    "8c9d4e2f-1a3b-4c5d-6e7f-8a9b0c1d2e3f"

// ===== LED 引脚 =====
#define PIN_RED    5
#define PIN_YELLOW 6
#define PIN_GREEN  7
#define PIN_BUZZER 4
#define PIN_BLUE   8  // 板载蓝灯，BLE 连接状态

// ===== LED 最大亮度（0-255）=====
#define BRIGHTNESS_RED    255
#define BRIGHTNESS_YELLOW 255
#define BRIGHTNESS_GREEN  255

// ===== 自动关闭配置 =====
#define AUTO_OFF_MS 300000  // 5 分钟 = 300000 毫秒

#endif
