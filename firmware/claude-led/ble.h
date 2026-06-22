#ifndef BLE_H
#define BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <esp_task_wdt.h>
#include "config.h"

// ===== BLE 状态 =====
BLEServer *pServer = NULL;
BLECharacteristic *pChar = NULL;
bool connected = false;
bool shouldAdvertise = false;
unsigned long advertiseTime = 0;

// ===== 前向声明 =====
bool handleCommand(String cmd);

// ===== BLE 回调 =====
class ServerCB : public BLEServerCallbacks {
  void onConnect(BLEServer *s) {
    connected = true;
    digitalWrite(PIN_BLUE, HIGH);  // 蓝灯亮
    Serial.println("Connected");
    esp_task_wdt_delete(NULL);
  }

  void onDisconnect(BLEServer *s) {
    connected = false;
    digitalWrite(PIN_BLUE, LOW);   // 蓝灯灭
    shouldAdvertise = true;
    advertiseTime = millis();
    Serial.println("Disconnected");
  }
};

class CharCB : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *c) {
    String v = c->getValue();
    if (v.length() > 0) {
      Serial.print("RX: ");
      Serial.println(v.c_str());
      if (!handleCommand(v)) {
        Serial.println("Unknown cmd");
      }
    }
  }
};

ServerCB serverCB;
CharCB charCB;

// ===== BLE 初始化 =====
void setupBLE() {
  Serial.println("Init BLE...");
  BLEDevice::init(DEVICE_NAME);
  BLEDevice::setPower(ESP_PWR_LVL_P6);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(&serverCB);

  BLEService *svc = pServer->createService(SERVICE_UUID);

  pChar = svc->createCharacteristic(
    CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pChar->setCallbacks(&charCB);

  svc->start();

  BLEAdvertising *adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->setScanResponse(true);
  adv->setMinPreferred(0x06);
  adv->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE ready!");
  Serial.print("Name: ");
  Serial.println(DEVICE_NAME);
  Serial.println("Waiting for connection...");
}

// ===== BLE 循环处理 =====
void updateBLE() {
  if (shouldAdvertise && millis() - advertiseTime > 1000) {
    shouldAdvertise = false;
    BLEDevice::startAdvertising();
    Serial.println("Advertising restarted");
  }
}

#endif
