#include "mail_service.h"
#include "wifi_service.h"
#include "secrets.h"
#include <Arduino.h>
#include <WiFiSSLClient.h>
namespace {
// 【可更改】重試／回應等待間隔，單位毫秒
constexpr unsigned long RETRY_MS = 60000UL;
constexpr unsigned long RESPONSE_MS = 8000UL;
// 【可更改】固定 ASCII 信件內容；若加入使用者輸入，需另做 header 與 dot escaping。
const char* subjects[] = {"Low water", "Soil too wet", "Soil sensor fault"};
const char* bodies[] = {
  "Water tank is low. Please refill the tank.",
  "Soil is too wet. Please check drainage and ventilation.",
  "Soil sensor reading is invalid. Please check wiring and calibration."
};
WiFiSSLClient client;
bool pending[ALERT_COUNT] = {}, sent[ALERT_COUNT] = {}, tried[ALERT_COUNT] = {};
unsigned long lastAttempt[ALERT_COUNT] = {};
int active = -1;
int step = 0;
unsigned long waitingSince = 0;
String line;
bool valid(AlertType type) { return type >= ALERT_LOW_WATER && type < ALERT_COUNT; }
String base64(const char* input) {
  const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String out;
  uint32_t bits = 0;
  int count = -6;
  for (const unsigned char* p = reinterpret_cast<const unsigned char*>(input); *p; ++p) {
    bits = (bits << 8) | *p;
    count += 8;
    while (count >= 0) { out += alphabet[(bits >> count) & 63]; count -= 6; }
  }
  if (count > -6) out += alphabet[((bits << 8) >> (count + 8)) & 63];
  while (out.length() % 4) out += '=';
  return out;
}
void finish(bool success) {
  client.stop();
  if (active >= 0) {
    tried[active] = true;
    lastAttempt[active] = millis();
    if (success) { sent[active] = true; pending[active] = false; }
  }
  Serial.println(success ? "SMTP accepted alert (delivery not guaranteed)." : "SMTP failed; retry after 60s while alert remains active.");
  active = -1;
  line = "";
}
void command(const String& value) {
  // 不輸出 SMTP 命令，避免洩漏 Base64 帳密。
  client.println(value);
  waitingSince = millis();
}
// 最多處理 128 bytes／圈，辨識多行回應的最後一行。
int response() {
  for (int n = 0; n < 128 && client.available(); ++n) {
    char c = client.read();
    if (c == '\n') {
      int code = 0;
      if (line.length() >= 4 && line[0] >= '0' && line[0] <= '9' &&
          line[1] >= '0' && line[1] <= '9' && line[2] >= '0' && line[2] <= '9' && line[3] == ' ')
        code = (line[0]-'0')*100 + (line[1]-'0')*10 + line[2]-'0';
      line = "";
      if (code) return code;
    } else if (c != '\r') {
      if (line.length() >= 512) return -1;
      line += c;
    }
  }
  if (millis() - waitingSince >= RESPONSE_MS || (!client.connected() && !client.available())) return -1;
  return 0;
}
}
void mailInit() {
  client.stop();
  active = -1;
  line = "";
  for (int i=0; i<ALERT_COUNT; ++i) { pending[i]=sent[i]=tried[i]=false; lastAttempt[i]=0; }
  Serial.println(GOOGLE_APP_PASSWORD[0] ? "Mail ready; no startup email." : "Mail disabled: fill GOOGLE_APP_PASSWORD in secrets.h.");
}
bool mailIsBusy() { return active >= 0; }
void sendAlert(AlertType type) { if (valid(type) && !sent[type]) pending[type]=true; }
void clearAlert(AlertType type) {
  if (!valid(type)) return;
  if (active == type) { client.stop(); active=-1; line=""; }
  pending[type]=sent[type]=tried[type]=false;
}
void mailUpdate() {
  if (!wifiIsConnected()) { if (active >= 0) finish(false); return; }
  if (!GOOGLE_APP_PASSWORD[0]) return;
  if (active < 0) {
    for (int i=0; i<ALERT_COUNT; ++i) {
      if (!pending[i] || sent[i] || (tried[i] && millis()-lastAttempt[i]<RETRY_MS)) continue;
      active=i; step=0; line="";
      // TLS connect 仍為同步呼叫；不可宣稱整個模組完全非阻塞。
      if (!client.connect(SMTP_HOST, SMTP_PORT)) { finish(false); return; }
      waitingSince=millis();
      return;
    }
    return;
  }
  const int code=response();
  if (!code) return;
  const int expected[] = {220,250,334,334,235,250,250,354,250};
  if (code != expected[step]) {
    Serial.print("SMTP response code: "); Serial.println(code);
    finish(false); return;
  }
  switch (step++) {
    case 0: command("EHLO smart-plant"); break;
    case 1: command("AUTH LOGIN"); break;
    case 2: command(base64(ALERT_EMAIL_FROM)); break;
    case 3: command(base64(GOOGLE_APP_PASSWORD)); break;
    case 4: command(String("MAIL FROM:<")+ALERT_EMAIL_FROM+">"); break;
    case 5: command(String("RCPT TO:<")+ALERT_EMAIL_TO+">"); break;
    case 6: command("DATA"); break;
    case 7:
      client.println(String("From: Smart Plant <")+ALERT_EMAIL_FROM+">");
      client.println(String("To: <")+ALERT_EMAIL_TO+">");
      client.println(String("Subject: Smart Plant - ")+subjects[active]);
      client.println("MIME-Version: 1.0");
      client.println("Content-Type: text/plain; charset=UTF-8");
      client.println();
      client.println(bodies[active]);
      command(".");
      break;
    case 8: client.println("QUIT"); finish(true); break;
  }
}
