#include "config.h"
#include "interfaces.h"

// 開機時先視為低水位，直到第一次安全檢查確認水位正常。
static bool lowWater = true;

void safetyInit() {
  pinMode(PIN_WATER_LEVEL, INPUT_PULLUP);
  pumpOff();
  lowWater = true;
}

bool isLowWater() {
  const int value = digitalRead(PIN_WATER_LEVEL);
  return WATER_LEVEL_ACTIVE_LOW ? value == LOW : value == HIGH;
}

bool checkWaterSafety() {
  lowWater = isLowWater();
  if (lowWater) {
    pumpOff();
    buzzerOn();
  } else {
    buzzerOff();
  }
  return lowWater;
}

bool isPumpAllowed() {
  return !lowWater;
}
