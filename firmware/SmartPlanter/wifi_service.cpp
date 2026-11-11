#include "wifi_service.h"
#if __has_include("secrets.h")
#include "secrets.h"
#else
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#endif
#include <WiFiS3.h>
#include <Arduino_LED_Matrix.h>
namespace {
ArduinoLEDMatrix matrix; // 本模組獨佔 LED matrix；其他模組勿同時繪圖
// 【可更改】重連間隔（毫秒）；圖案沿用原 wifi_test
constexpr unsigned long RETRY_MS = 30000UL;
unsigned long lastAttempt = 0;
bool attempted = false;
bool wasConnected = false;
// Wi-Fi 成功：打勾 ✓
uint8_t wifiOK[8][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,1,0,0,0,1,0,0,0,1,0,0},
  {0,0,1,0,1,0,1,0,1,0,0,0},
  {0,0,0,1,0,0,0,1,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};

// 尚未連線：三個點 ...
uint8_t connecting[8][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,1,1,0,1,1,0,1,1,0,0},
  {0,0,1,1,0,1,1,0,1,1,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0}
};

}
void wifiInit() {
  matrix.begin();
  matrix.renderBitmap(connecting, 8, 12);
  attempted = false;
  wasConnected = false;
}
bool wifiIsConnected() { return WiFi.status() == WL_CONNECTED; }
void wifiUpdate() {
  const bool connected = wifiIsConnected();
  if (connected != wasConnected) {
    if (connected) {
      matrix.renderBitmap(wifiOK, 8, 12);
      Serial.print("WiFi connected. IP: ");
      Serial.println(WiFi.localIP());
    } else {
      matrix.renderBitmap(connecting, 8, 12);
      Serial.println("WiFi disconnected.");
    }
    wasConnected = connected;
  }
  if (connected) return;
  if ((!attempted || millis() - lastAttempt >= RETRY_MS) && WIFI_SSID[0]) {
    attempted = true;
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // 底層呼叫仍可能阻塞
    lastAttempt = millis();
  }
}
