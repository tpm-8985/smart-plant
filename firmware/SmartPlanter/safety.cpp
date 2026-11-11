
#include "config.h"
#include "interfaces.h"

// 上次安全檢查的結果
// 開機預設不允許啟動水泵
static bool lowWater = true;

void safetyInit() {
    pinMode(PIN_WATER_LEVEL, INPUT_PULLUP);

    // 開機先關閉水泵
    pumpOff();
    lowWater = true;
}

bool isLowWater() {
    int value = digitalRead(PIN_WATER_LEVEL);

    if (WATER_LEVEL_ACTIVE_LOW) {
        return value == LOW;
    }

    return value == HIGH;
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
<<<<<<< Updated upstream
  // TODO(娟華)
  return false;  // 暫時的回傳值：先回傳「不允許」，實作完成前水泵不會被開啟，實作後刪除
=======
    return !lowWater;
>>>>>>> Stashed changes
}
