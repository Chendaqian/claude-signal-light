/*
  LED 板测试代码
  依次点亮红、黄、绿灯，每个亮1秒
*/

#define PIN_RED    5   // 红灯
#define PIN_YELLOW 6   // 黄灯
#define PIN_GREEN  7   // 绿灯

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);

  // 先全部熄灭
  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_YELLOW, LOW);
  digitalWrite(PIN_GREEN, LOW);

  Serial.println("LED test started");
}

void loop() {
  // 红灯亮1秒
  Serial.println("Red ON");
  digitalWrite(PIN_RED, HIGH);
  delay(1000);
  digitalWrite(PIN_RED, LOW);

  // 黄灯亮1秒
  Serial.println("Yellow ON");
  digitalWrite(PIN_YELLOW, HIGH);
  delay(1000);
  digitalWrite(PIN_YELLOW, LOW);

  // 绿灯亮1秒
  Serial.println("Green ON");
  digitalWrite(PIN_GREEN, HIGH);
  delay(1000);
  digitalWrite(PIN_GREEN, LOW);

  delay(500); // 间隔0.5秒
}
