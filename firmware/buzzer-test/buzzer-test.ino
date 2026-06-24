/*
  蜂鸣器测试固件（支持有源/无源）
  ESP32-C3 GPIO4

  串口 115200 查看状态

  测试模式：
  - MODE 0: 有源蜂鸣器测试（高电平触发）
  - MODE 1: 无源蜂鸣器测试（PWM驱动）

  每个测试间隔 1 秒，循环执行
*/

#define PIN_BUZZER 4
#define BUZZER_FREQ 2000
#define BUZZER_RESOLUTION 8

// 有源蜂鸣器：直接给高电平
void beepActive(int durationMs) {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(durationMs);
  digitalWrite(PIN_BUZZER, LOW);
}

// 无源蜂鸣器：PWM驱动
void beepPassive(int durationMs) {
  ledcWriteTone(PIN_BUZZER, BUZZER_FREQ);
  delay(durationMs);
  ledcWriteTone(PIN_BUZZER, 0);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== Buzzer Test ===");
  Serial.println("GPIO4 initialization...");
  Serial.println();

  // 初始化引脚为输出
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  Serial.print("Pin mode set to OUTPUT. Initial state: ");
  Serial.println(digitalRead(PIN_BUZZER));

  // 初始化PWM（备用）
  ledcAttach(PIN_BUZZER, BUZZER_FREQ, BUZZER_RESOLUTION);
  ledcWriteTone(PIN_BUZZER, 0);

  delay(500);
  Serial.println("Ready! Starting tests...");
  Serial.println();
}

void loop() {
  // ===== 测试1: 有源蜂鸣器模式（高电平触发）=====
  Serial.println("=== Test 1: Active Buzzer (HIGH level) ===");

  Serial.print("  Short beep (100ms)... GPIO4=");
  Serial.print(digitalRead(PIN_BUZZER));
  Serial.print(" → ");
  digitalWrite(PIN_BUZZER, HIGH);
  Serial.print(digitalRead(PIN_BUZZER));
  delay(100);
  digitalWrite(PIN_BUZZER, LOW);
  Serial.print(" → ");
  Serial.println(digitalRead(PIN_BUZZER));
  delay(500);

  // ===== 测试3: 无源蜂鸣器模式（PWM）=====
  Serial.println("=== Test 3: Passive Buzzer (PWM) ===");

  Serial.print("  2000Hz beep (200ms)... ");
  ledcWriteTone(PIN_BUZZER, 2000);
  delay(200);
  ledcWriteTone(PIN_BUZZER, 0);
  delay(500);

  Serial.print("  1000Hz beep (200ms)... ");
  ledcWriteTone(PIN_BUZZER, 1000);
  delay(200);
  ledcWriteTone(PIN_BUZZER, 0);
  delay(500);

  Serial.print("  4000Hz beep (200ms)... ");
  ledcWriteTone(PIN_BUZZER, 4000);
  delay(200);
  ledcWriteTone(PIN_BUZZER, 0);
  delay(1000);

  // ===== 直接测试：长时间响 =====
  Serial.println("=== Test 4: Direct long test (1 second) ===");
  Serial.println("  Turning ON GPIO4 HIGH for 1 second...");
  digitalWrite(PIN_BUZZER, HIGH);
  Serial.print("  GPIO4 state: ");
  Serial.println(digitalRead(PIN_BUZZER));
  delay(1000);
  digitalWrite(PIN_BUZZER, LOW);
  Serial.println("  GPIO4 turned OFF");

  Serial.println("=== Test 5: PWM long test (1 second) ===");
  Serial.println("  PWM 2000Hz for 1 second...");
  ledcWriteTone(PIN_BUZZER, 2000);
  delay(1000);
  ledcWriteTone(PIN_BUZZER, 0);
  Serial.println("  PWM stopped");

  Serial.println("--- Test cycle complete ---");
  Serial.println();
  delay(1000);
}
