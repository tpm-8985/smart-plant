// config.h — 腳位、校正值與控制參數　負責人：每一項各自標註
//
// 好幾個模組都會用到的常數集中定義在這裡，大家用同一個名稱，避免各寫各的數字。
// 下面列出預計需要的常數，數值由負責人接線、實測或決定之後，
// 把那一行的註解取消，再把 ? 換成實際的值。
// 這是全組共用的檔案，修改前先在群組說一聲。
#pragma once
#include <Arduino.h>

// ===== 功能開關（建瑄）=====
// #define SIMULATE_SENSORS ?   // 1：感測模組改回傳假資料，沒有接硬體也能測試流程
// #define DEBUG_PRINT      ?   // 1：在 Serial Monitor 定期印出系統狀態

// ===== 腳位（接線圖確定後填入）=====
// 腳位以最終成品那一塊 UNO R4 WiFi 為準。Board B、C 測試時也照這組腳位接線，
// 整合到同一塊板子時才不用改程式，也不會有兩個模組搶同一支腳。
// const uint8_t PIN_SOIL_SENSOR  = ?;  // 淯瑋：土壤感測器的類比輸出
// const uint8_t PIN_SOIL_POWER   = ?;  // 淯瑋：感測器供電腳位（如果採用只在量測時通電）
// const uint8_t PIN_WATER_LEVEL  = ?;  // 娟華：浮球開關
// const uint8_t PIN_BUZZER       = ?;  // 娟華：有源蜂鳴器
// const uint8_t PIN_PUMP_RELAY   = ?;  // 玟君：水泵的繼電器
// const uint8_t PIN_FAN_RELAY    = ?;  // 玟君：風扇的繼電器
// const bool    RELAY_ACTIVE_LOW = ?;  // 玟君：繼電器是否為低電位觸發（true / false）

// ===== 感測器校正（淯瑋：實測後填入）=====
// 注意：校正值最後要在成品那一塊 UNO R4 WiFi 上再量一次。
//      測試用的相容板，類比讀值的基準電壓可能略有不同，換板後同一盆土的讀值會有差異。
// const int SOIL_RAW_DRY = ?;  // 乾燥端點的原始讀值
// const int SOIL_RAW_WET = ?;  // 濕潤端點的原始讀值

// ===== 量測（建瑄）=====
// const unsigned long SAMPLE_INTERVAL_MS = ?;  // 多久量一次土壤濕度（毫秒）

// ===== 控制參數（明杰：依實驗決定）=====
// const int MOISTURE_DRY    = ?;  // 低於此值開始補水 (%)
// const int MOISTURE_TARGET = ?;  // 補水的目標濕度 (%)
// const int MOISTURE_WET    = ?;  // 高於此值啟動風扇 (%)；通知模組也用這個值判斷「過濕」
// const unsigned long SOAK_WAIT_MS = ?;  // 補水後等待滲透的時間（毫秒）
// 自適應澆水、遲滯等其他需要的參數，由明杰自行新增在這一段
