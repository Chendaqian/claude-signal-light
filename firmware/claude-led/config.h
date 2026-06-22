#ifndef CONFIG_H
#define CONFIG_H

// ===== BLE 配置 =====
#define DEVICE_NAME "Claude-LED-JGY"
#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHAR_UUID "12345678-1234-1234-1234-123456789abd"

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
