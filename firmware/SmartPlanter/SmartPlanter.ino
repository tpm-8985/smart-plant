// SmartPlanter.ino — 主程式　負責人：建瑄
//
// 這個檔案的工作：
//   依照 docs/main-flow.md 的主迴圈流程圖，依序呼叫各模組的函式。
//   各功能的細節寫在各自的 .cpp 檔，主程式只負責「什麼時候呼叫誰」。
//
// 主程式要保存的資料（變數名稱自訂）：
//   - 最新的土壤濕度（-1 代表讀值異常）
//   - 上一次量測的時間，用來判斷是否滿 SAMPLE_INTERVAL_MS
//
// 時程（依進度規劃表）：
//   10/14–10/20：Board A 基本程式、Serial Monitor 輸出、主迴圈骨架
//   10/21–10/27：把各模組的函式接進主迴圈
//   10/28–11/03：整合所有模組，處理腳位與時間衝突
//   11/04–11/10：整理狀態輸出，確認整個 loop 沒有阻塞
#include "config.h"
#include "interfaces.h"

// 模擬模式的假資料，宣告在 interfaces.h，由 handleSerialCommands() 設定
int  simMoisture;
bool simLowWater;


// ------------------------------------------------------------
// void setup()
// 功能：開機時執行一次，初始化所有模組。
// 流程：
//   1. 啟動 Serial（鮑率自訂，Serial Monitor 要設成一樣）
//   2. 最先呼叫 actuatorsInit()，確保開機時水泵、風扇都是關的
//   3. 呼叫 soilInit()、safetyInit()、controlInit()、notifyInit()
//   4. 先量一次土壤濕度，讓第一圈 loop 就有值可用
//   5. 模擬模式時，印出可用的 Serial 指令
// ------------------------------------------------------------
void setup() {
  // TODO(建瑄)
}


// ------------------------------------------------------------
// void loop()
// 功能：不斷重複執行，每一圈依序做四段（見主迴圈流程圖）。
// 流程：
//   0. 模擬模式時，先呼叫 handleSerialCommands() 讀取指令
//   1. 安全檢查：呼叫 checkWaterSafety()，記下這一圈是否低水位
//   2. 量測：距上次量測滿 SAMPLE_INTERVAL_MS，就呼叫 readSoilMoisture() 更新最新濕度，
//            否則沿用上次的值；DEBUG_PRINT 開啟時，量測後呼叫 printStatus()
//   3. 控制：每一圈都呼叫 controlUpdate(最新濕度)，即使這圈沒有新讀值
//   4. 通知：呼叫 updateAlerts(是否低水位, 最新濕度)，再呼叫 notifyUpdate()
// 規則：整個 loop 不可使用 delay() 長時間等待，否則安全檢查會被耽誤。
// ------------------------------------------------------------
void loop() {
  // TODO(建瑄)
}


// ------------------------------------------------------------
// void updateAlerts(bool lowWater, int moisture)
// 功能：主迴圈第 ④ 段，決定要發出或解除哪些通知。
// 流程：
//   1. 低水位：是 → sendAlert(ALERT_LOW_WATER)；否 → clearAlert(ALERT_LOW_WATER)
//   2. 感測器異常（moisture 為 -1）：是 → sendAlert(ALERT_SENSOR_FAULT)；否 → clearAlert
//   3. 過濕（moisture 大於 MOISTURE_WET）：是 → sendAlert(ALERT_TOO_WET)；否 → clearAlert
//      讀值異常時無法判斷是否過濕，這一項先不處理
// 備註：「只寄一次」由通知模組負責，這裡每一圈都可以呼叫。
// ------------------------------------------------------------
void updateAlerts(bool lowWater, int moisture) {
  // TODO(建瑄)
}


// ------------------------------------------------------------
// void printStatus()
// 功能：在 Serial Monitor 印出一行系統狀態，方便除錯與記錄數據。
// 內容建議：土壤濕度、是否低水位、水泵與風扇開關、控制狀態機目前的狀態
//          （狀態名稱用 controlStateName() 取得）
// ------------------------------------------------------------
void printStatus() {
  // TODO(建瑄)
}


// ------------------------------------------------------------
// void handleSerialCommands()
// 功能：模擬模式下，讀取 Serial Monitor 輸入的指令，設定 simMoisture 與 simLowWater，
//      讓還沒有硬體的組員也能測試整個流程。
// 流程：
//   1. 讀取目前已收到的字元，收到換行代表一行指令結束
//   2. 解析指令，例如設定濕度、設定是否低水位（指令格式自訂，寫進 README）
//   3. 印出收到的指令，方便確認
// 規則：只讀取已經收到的字元，不可停下來等待輸入。
// ------------------------------------------------------------
void handleSerialCommands() {
  // TODO(建瑄)
}
