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

#include "config.h"
#include "interfaces.h"

// 模擬感測器資料
int simMoisture = 50;
bool simLowWater = false;

// 最新讀值及量測時間
int latestMoisture = -1;
unsigned long lastSampleAt = 0;

// Serial 指令緩衝區
String serialLine;

// 函式宣告
void updateAlerts(bool lowWater, int moisture);
void printStatus();
void handleSerialCommands();
// 模擬模式的假資料，宣告在 interfaces.h，由 handleSerialCommands() 設定

// 最新濕度與上次量測時間
int latestMoisture = -1;
unsigned long lastSampleAt = 0;

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
<<<<<<< Updated upstream

  Serial.begin(9600);

  actuatorsInit();
  soilInit();
  safetyInit();
  controlInit();
  notifyInit();

  latestMoisture = readSoilMoisture();
  lastSampleAt = millis();

  printStatus();
=======
  Serial.begin(115200);

    // 先初始化致動器，目標是全部關閉
    actuatorsInit();

    soilInit();
    safetyInit();
    controlInit();
    wifiInit();
    mailInit();

    // 取得第一筆濕度
    latestMoisture = readSoilMoisture();
    lastSampleAt = millis();

    Serial.println("Smart Planter started");

#if SIMULATE_SENSORS
    Serial.println("Simulation mode");
    Serial.println("M=20  Set moisture to 20%");
    Serial.println("W=1   Simulate low water");
    Serial.println("W=0   Simulate normal water");
#endif

    printStatus();
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream

  #if SIMULATE_SENSORS
    handleSerialCommands();
  #endif

    bool lowWater = checkWaterSafety();

    unsigned long now = millis();

    if ((unsigned long)(now - lastSampleAt) >= SAMPLE_INTERVAL_MS) {
      lastSampleAt = now;
      latestMoisture = readSoilMoisture();
      printStatus();
    }

    controlUpdate(latestMoisture);
    updateAlerts(lowWater, latestMoisture);
    notifyUpdate();

=======
  // 0. 模擬模式：處理 Serial 指令
#if SIMULATE_SENSORS
    handleSerialCommands();
#endif

    // 1. 安全檢查
    bool lowWater = checkWaterSafety();

    // 2. 定時讀取土壤濕度
    unsigned long now = millis();
    bool newSample = false;

    if (now - lastSampleAt >= SAMPLE_INTERVAL_MS) {
        lastSampleAt = now;
        latestMoisture = readSoilMoisture();
        newSample = true;
    }

    // 3. 控制狀態機
    controlUpdate(latestMoisture);

    // 4. 定期輸出目前狀態
#if DEBUG_PRINT
    if (newSample) {
        printStatus();
    }
#endif

    // 5. 異常通知
    updateAlerts(lowWater, latestMoisture);
    wifiUpdate();
    mailUpdate();
>>>>>>> Stashed changes
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
  // 水箱低水位
    if (lowWater) {
        sendAlert(ALERT_LOW_WATER);
    } else {
        clearAlert(ALERT_LOW_WATER);
    }

    // 土壤感測器故障
    if (moisture < 0 || moisture > 100) {
        sendAlert(ALERT_SENSOR_FAULT);
    } else {
        clearAlert(ALERT_SENSOR_FAULT);
    }

    // 土壤過濕
    // 感測器故障時無法可靠判定
    if (moisture >= 0 && moisture <= 100) {
        if (moisture > MOISTURE_WET) {
            sendAlert(ALERT_TOO_WET);
        } else {
            clearAlert(ALERT_TOO_WET);
        }
    }
}


// ------------------------------------------------------------
// void printStatus()
// 功能：在 Serial Monitor 印出一行系統狀態，方便除錯與記錄數據。
// 內容建議：土壤濕度、是否低水位、水泵與風扇開關、控制狀態機目前的狀態
//          （狀態名稱用 controlStateName() 取得）
// ------------------------------------------------------------
void printStatus() {
  // TODO(建瑄)
<<<<<<< Updated upstream

  Serial.print("Soil moisture: ");

  if (latestMoisture == -1) {
    Serial.println("SENSOR_FAULT");
  } else {
    Serial.print(latestMoisture);
    Serial.println("%");
  }
=======
  Serial.print("Moisture: ");
    Serial.print(latestMoisture);
    Serial.print("% | ");

    Serial.print("Low Water: ");
    Serial.print(isLowWater());
    Serial.print(" | ");

    Serial.print("Pump: ");
    Serial.print(isPumpOn());
    Serial.print(" | ");

    Serial.print("Fan: ");
    Serial.print(isFanOn());
    Serial.print(" | ");

    Serial.print("State: ");
    Serial.println(controlStateName());
>>>>>>> Stashed changes
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
  while (Serial.available() > 0) {

        char c = Serial.read();

        if (c == '\n' || c == '\r') {

            if (serialLine.length() == 0) {
                continue;
            }

            serialLine.trim();

            if (serialLine.startsWith("M=")) {
                int value =
                    serialLine.substring(2).toInt();

                if (value >= -1 && value <= 100) {
                    simMoisture = value;
                    Serial.print("Set moisture: ");
                    Serial.println(simMoisture);
                } else {
                    Serial.println("Invalid moisture");
                }
            }

            else if (serialLine == "W=1") {
                simLowWater = true;
                Serial.println("Low water simulated");
            }

            else if (serialLine == "W=0") {
                simLowWater = false;
                Serial.println("Water level normal");
            }

            else {
                Serial.println("Unknown command");
            }

            serialLine = "";
        }

        else if (serialLine.length() < 40) {
            serialLine += c;
        }
    }
}
