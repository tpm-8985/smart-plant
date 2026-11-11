
#include "config.h"
#include "interfaces.h"

// 記錄致動器的軟體狀態
static bool pumpRunning = false;
static bool fanRunning = false;

// 繼電器電位設定
// RELAY_ACTIVE_LOW = true：LOW 開啟、HIGH 關閉
static uint8_t relayOnLevel() {
    return RELAY_ACTIVE_LOW ? LOW : HIGH;
}

static uint8_t relayOffLevel() {
    return RELAY_ACTIVE_LOW ? HIGH : LOW;
}

// ======================================
// 初始化所有致動器
// ======================================
void actuatorsInit() {

    // 1. 水泵：先設定關閉電位
    digitalWrite(PIN_PUMP_RELAY, relayOffLevel());
    pinMode(PIN_PUMP_RELAY, OUTPUT);

    // 2. 風扇：兩個控制訊號都設 LOW
    digitalWrite(PIN_FAN_INA, LOW);
    digitalWrite(PIN_FAN_INB, LOW);

    pinMode(PIN_FAN_INA, OUTPUT);
    pinMode(PIN_FAN_INB, OUTPUT);

    // 3. 蜂鳴器：依原本 PR #2，HIGH 表示關閉
    digitalWrite(PIN_BUZZER, HIGH);
    pinMode(PIN_BUZZER, OUTPUT);

    // 4. 更新軟體狀態
    pumpRunning = false;
    fanRunning = false;
}

// ======================================
// 水泵控制
// ======================================
void pumpOn() {
    digitalWrite(PIN_PUMP_RELAY, relayOnLevel());
    pumpRunning = true;
}

void pumpOff() {
    digitalWrite(PIN_PUMP_RELAY, relayOffLevel());
    pumpRunning = false;
}

bool isPumpOn() {
    return pumpRunning;
}

// ======================================
// 風扇控制
// ======================================
void fanOn() {
    digitalWrite(PIN_FAN_INA, LOW);
    digitalWrite(PIN_FAN_INB, HIGH);

    fanRunning = true;
}

void fanOff() {
    digitalWrite(PIN_FAN_INA, LOW);
    digitalWrite(PIN_FAN_INB, LOW);

    fanRunning = false;
}

bool isFanOn() {
    return fanRunning;
}

// ======================================
// 蜂鳴器控制
// ======================================
void buzzerOn() {
    digitalWrite(PIN_BUZZER, LOW);
}

void buzzerOff() {
    digitalWrite(PIN_BUZZER, HIGH);
}
