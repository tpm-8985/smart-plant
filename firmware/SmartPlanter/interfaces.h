// interfaces.h — 全組共用的函式介面　負責人：建瑄
//
// 這裡列出每個模組對外提供的函式。名稱、參數與回傳值是全組的約定，
// 大家都照這些名稱呼叫，各自的實作寫在自己的 .cpp 檔。
// 每個函式的詳細規格（要做什麼、流程、注意事項）寫在對應 .cpp 檔的函式上方。
//
// 要修改這個檔案，請先在週三會議討論，因為改了這裡，其他人的程式也要跟著改。
#pragma once
#include <Arduino.h>

// ---------- 土壤感測（淯瑋）soil_sensor.cpp ----------
void soilInit();             // 開機初始化
int  readSoilRaw();          // 讀取原始類比值（校正時使用）
int  readSoilMoisture();     // 回傳土壤濕度 0–100 (%)；讀值異常時回傳 -1

// ---------- 致動器（玟君）actuators.cpp ----------
void actuatorsInit();        // 開機初始化，所有致動器設為關閉
void pumpOn();               // 開水泵
void pumpOff();              // 關水泵
bool isPumpOn();             // 水泵目前是否開著
void fanOn();                // 開風扇
void fanOff();               // 關風扇
bool isFanOn();              // 風扇目前是否開著
void buzzerOn();             // 蜂鳴器響
void buzzerOff();            // 蜂鳴器停

// ---------- 水位與安全（娟華）safety.cpp ----------
void safetyInit();           // 開機初始化
bool isLowWater();           // 水箱是否低於安全水位
bool checkWaterSafety();     // 每圈最先呼叫：低水位時停泵並提醒，回傳是否低水位
bool isPumpAllowed();        // 目前是否允許開水泵

// ---------- 控制狀態機（明杰）control.cpp ----------
void controlInit();               // 開機初始化
void controlUpdate(int moisture); // 每圈呼叫一次，狀態機走一步
const char* controlStateName();   // 目前狀態的名稱（除錯用）

// ---------- 通知（承儒）notify.cpp ----------
// 三種需要通知的異常
enum AlertType { ALERT_LOW_WATER, ALERT_TOO_WET, ALERT_SENSOR_FAULT, ALERT_COUNT };
void notifyInit();                 // 開機初始化（連上 Wi-Fi）
void notifyUpdate();               // 每圈呼叫（維持 Wi-Fi 連線）
void sendAlert(AlertType type);    // 發出通知；同一種異常在恢復前只寄一次
void clearAlert(AlertType type);   // 異常恢復正常時呼叫，允許下次再寄

// ---------- 模擬模式的假資料（建瑄）SmartPlanter.ino ----------
// 沒有接感測器時，感測模組改回傳這兩個值，由主程式透過 Serial 指令設定
extern int  simMoisture;     // 假的土壤濕度 (%)
extern bool simLowWater;     // 假的水箱低水位狀態
