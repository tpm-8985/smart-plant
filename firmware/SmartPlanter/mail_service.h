#pragma once
// 【固定介面】不得與舊 interfaces.h 的同名 enum 重複宣告。
enum AlertType { ALERT_LOW_WATER, ALERT_TOO_WET, ALERT_SENSOR_FAULT, ALERT_COUNT };
void mailInit();                 // setup：初始化，不自動寄信
void sendAlert(AlertType type);  // 只排隊；同一異常成功送出後不重複寄
void clearAlert(AlertType type); // 恢復：取消待寄／進行中通知並允許下次寄送
void mailUpdate();               // loop：逐步處理 SMTP，一次最多一封進行中
bool mailIsBusy();               // 是否正在進行 SMTP 傳輸
