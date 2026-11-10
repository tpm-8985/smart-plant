// control.cpp — 控制狀態機　負責人：明杰
//
// 這個檔案的工作：
//   依照土壤濕度，決定何時澆水、何時風乾。所有致動決策都在這個狀態機裡，
//   狀態圖見 docs/main-flow.md「控制狀態機」。
//   自適應澆水與遲滯是本專題主要的自主設計，報告時要能說明設計方法與實驗結果。
//
// 會用到 config.h 的常數：
//   MOISTURE_DRY、MOISTURE_TARGET、MOISTURE_WET、SOAK_WAIT_MS，以及自行新增的參數
//
// 時程（依進度規劃表）：
//   10/07–10/13：用假資料寫判斷，初步規劃過乾、正常、過濕的門檻與澆水時間
//   10/14–10/20：依濕度判斷乾、正常、過濕，用 Serial 印出預期動作
//   10/21–10/27：第一版控制策略，依乾燥程度決定澆水時間，定義啟動風扇的條件
//   10/28–11/03：調整門檻，加入澆水後等待滲透再量測
//   11/04–11/10：加入遲滯，避免在門檻附近頻繁開關
//
// 規則：這個檔案裡不能用 delay() 等待，計時一律用 millis()。
//      否則澆水或等待滲透的期間，安全檢查和通知都會停住。
#include <Arduino.h>
#include "config.h"
#include "interfaces.h"

// ============================================================
// 控制參數
// config.h 目前尚未定義實際數值，因此先在這裡設定預設值。
// 後續應依照土壤、花盆與水泵的實測結果調整。
// ============================================================

// 濕度門檻（百分比）
//static constexpr int MOISTURE_DRY = 30;
//static constexpr int MOISTURE_TARGET = 45;
//static constexpr int MOISTURE_WET = 70;

// 風扇停止門檻：低於啟動門檻，形成遲滯區間
static constexpr int CONTROL_DRYING_STOP =
    MOISTURE_WET - 5;

// 澆水時間（毫秒）
static constexpr unsigned long CONTROL_MIN_WATER_MS = 1000UL;
static constexpr unsigned long CONTROL_MAX_WATER_MS = 8000UL;

// 澆水後等待水分滲透的時間
//static constexpr unsigned long SOAK_WAIT_MS = 30000UL;

// ============================================================
// 狀態定義
// ============================================================

enum ControlState {
  STATE_IDLE,
  STATE_WATERING,
  STATE_SOAKING,
  STATE_DRYING,
  STATE_FAULT
};

static ControlState currentState = STATE_IDLE;

// 記錄目前狀態開始的時間
static unsigned long stateStartTime = 0;

// 本次預計澆水時間
static unsigned long wateringDurationMs = 0;

// ============================================================
// 輔助函式：切換狀態
// ============================================================

static void enterState(ControlState nextState) {
  currentState = nextState;
  stateStartTime = millis();
}

// 確保水泵和風扇不會同時運轉
static void stopPumpAndFan() {
  pumpOff();
  fanOff();
}

// ============================================================
// 輔助函式：依缺水程度計算澆水時間
//
// 濕度越低，澆水時間越長。
// 濕度接近乾燥門檻時，使用較短的時間。
// ============================================================

static unsigned long calculateWateringDuration(int moisture) {

    const int deficit =
        MOISTURE_TARGET - moisture;

    const int maxDeficit =
        MOISTURE_TARGET;

    if (maxDeficit <= 0) {
        return CONTROL_MIN_WATER_MS;
    }

    long percentage =
        (static_cast<long>(deficit) * 100L) / maxDeficit;

    if (percentage < 0) {
        percentage = 0;
    }

    if (percentage > 100) {
        percentage = 100;
    }

    const unsigned long durationRange =
        CONTROL_MAX_WATER_MS - CONTROL_MIN_WATER_MS;

    return CONTROL_MIN_WATER_MS
        + (durationRange * static_cast<unsigned long>(percentage))
            / 100UL;
}

// ============================================================
// controlInit()
// 開機初始化：進入 IDLE，確保致動器關閉。
// ============================================================

void controlInit() {
  stopPumpAndFan();

  wateringDurationMs = 0;
  stateStartTime = millis();
  currentState = STATE_IDLE;
}

// ============================================================
// controlUpdate(int moisture)
// 每次呼叫只執行一次狀態判斷，不使用 delay()。
//
// moisture：0～100 表示濕度百分比；-1 表示讀值異常。
// 其他超出範圍的數值也視為異常。
// ============================================================

void controlUpdate(int moisture) {
  const unsigned long now = millis();

  // ----------------------------------------------------------
  // 第一優先：感測器故障保護
  // 無論目前在哪個狀態，異常時都立即停止水泵與風扇。
  // ----------------------------------------------------------

  if (moisture < 0 || moisture > 100) {
    stopPumpAndFan();
    wateringDurationMs = 0;

    if (currentState != STATE_FAULT) {
      enterState(STATE_FAULT);
    }

    return;
  }

  // 故障恢復後回到 IDLE，重新判斷環境
  if (currentState == STATE_FAULT) {
    stopPumpAndFan();
    enterState(STATE_IDLE);
    return;
  }

  // ----------------------------------------------------------
  // 狀態機
  // ----------------------------------------------------------

  switch (currentState) {

    // --------------------------------------------------------
    // IDLE：待機
    // --------------------------------------------------------
    case STATE_IDLE: {
      stopPumpAndFan();

      // 土壤太乾，而且水泵目前允許運作
      if (moisture < MOISTURE_DRY) {
        if (isPumpAllowed()) {
          wateringDurationMs =
              calculateWateringDuration(moisture);

          // 開泵前再次確認安全條件
          if (isPumpAllowed()) {
            fanOff();
            pumpOn();
            enterState(STATE_WATERING);
          }
        }
      }

      // 土壤太濕，啟動風扇
      else if (moisture > MOISTURE_WET) {
        pumpOff();
        fanOn();
        enterState(STATE_DRYING);
      }

      break;
    }

    // --------------------------------------------------------
    // WATERING：澆水
    // 澆水時間到，或水泵不再被允許運作，就停止澆水。
    // --------------------------------------------------------
    case STATE_WATERING: {
      // 感測器故障已在 switch 前處理。
      // 水位安全由 isPumpAllowed() 判斷。
      if (!isPumpAllowed()) {
        pumpOff();
        fanOff();

        // 等待已澆入的水滲透，再重新判斷
        enterState(STATE_SOAKING);
      }
      else if (now - stateStartTime >= wateringDurationMs) {
        pumpOff();
        fanOff();
        enterState(STATE_SOAKING);
      }

      break;
    }

    // --------------------------------------------------------
    // SOAKING：等待滲透
    // 此期間不根據濕度啟動新的澆水動作。
    // --------------------------------------------------------
    case STATE_SOAKING: {
      stopPumpAndFan();

      if (now - stateStartTime >= SOAK_WAIT_MS) {
        enterState(STATE_IDLE);
      }

      break;
    }

    // --------------------------------------------------------
    // DRYING：風乾
    // 使用不同的啟動與停止門檻，避免風扇頻繁開關。
    // --------------------------------------------------------
    case STATE_DRYING: {
      pumpOff();
      fanOn();

      if (moisture <= CONTROL_DRYING_STOP) {
        fanOff();
        enterState(STATE_IDLE);
      }

      break;
    }

    // --------------------------------------------------------
    // FAULT：故障
    // --------------------------------------------------------
    case STATE_FAULT: {
      stopPumpAndFan();
      break;
    }
  }
}

// ============================================================
// controlStateName()
// 回傳目前狀態的名稱，供 Serial Monitor 除錯使用。
// ============================================================

const char* controlStateName() {
  switch (currentState) {
    case STATE_IDLE:
      return "IDLE";

    case STATE_WATERING:
      return "WATERING";

    case STATE_SOAKING:
      return "SOAKING";

    case STATE_DRYING:
      return "DRYING";

    case STATE_FAULT:
      return "FAULT";

    default:
      return "UNKNOWN";
  }
}
