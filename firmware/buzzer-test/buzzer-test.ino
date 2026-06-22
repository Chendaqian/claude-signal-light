/*
  无源蜂鸣器测试固件
  ESP32-C3 GPIO4

  串口 115200 查看状态
  蜂鸣器响 1 秒 → 停 1 秒 → 循环
*/

#define PIN_BUZZER 4
#define BUZZER_FREQ 2048
#define BUZZER_RESOLUTION 8

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== Buzzer Test ===");

  // 初始化蜂鸣器
  ledcAttach(PIN_BUZZER, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcWriteTone(PIN_BUZZER, 0);
  Serial.println("Buzzer initialized, starting loop...");
}

void loop() {
  // 响 1 秒
  Serial.println("Buzz ON");
  ledcWriteTone(PIN_BUZZER, BUZZER_FREQ);
  delay(1000);

  // 停 1 秒
  Serial.println("Buzz OFF");
  ledcWriteTone(PIN_BUZZER, 0);
  delay(1000);
}
