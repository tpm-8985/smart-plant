# 主程式流程

## 主迴圈

loop() 一秒會跑上萬圈。每一圈依序做四件事，每件事都只做一小段就往下走，不會用 `delay()` 停下來等，所以安全檢查每一圈都會執行。

```mermaid
flowchart TD
    S([開機 setup]) --> S1[關閉所有致動器<br/>初始化各模組<br/>先量一次土壤濕度]
    S1 --> A

    subgraph P1 [① 安全檢查：每圈最先做]
        A{水箱低水位？}
        A -- 是 --> A1[強制關水泵<br/>蜂鳴器響]
        A -- 否 --> A2[關閉蜂鳴器]
    end

    subgraph P2 [② 量測：每隔固定時間一次]
        B{距上次量測<br/>滿量測間隔？}
        B -- 否 --> B1[沿用上次的濕度]
        B -- 是 --> C[讀取土壤濕度]
        C --> D{讀值異常？}
        D -- 是 --> D1[濕度記為「異常」]
        D -- 否 --> D2[更新最新濕度]
    end

    subgraph P3 [③ 控制]
        E[控制狀態機走一步<br/>依目前狀態判斷與動作<br/>見下方狀態圖]
    end

    subgraph P4 [④ 通知]
        N[低水位、過濕、感測器異常<br/>新發生的寄一次 Email<br/>恢復正常就重置<br/>維持 Wi-Fi 連線]
    end

    A1 --> B
    A2 --> B
    B1 --> E
    D1 --> E
    D2 --> E
    E --> N
    N -- 下一圈 --> A
```

- **安全檢查**放在最前面：低水位時直接關水泵，不管控制狀態機現在在哪個狀態。
- **量測**每隔固定時間（`SAMPLE_INTERVAL_MS`）才讀一次感測器，其他圈沿用上次的值。讀感測器需要一點時間，而且土壤濕度變化很慢，沒必要每圈都讀；如果感測器採用只在量測時通電，也能減少腐蝕。
- **控制**每一圈都要執行，即使這圈沒有新讀值，因為澆水時間要準時結束。
- **通知**集中在最後處理，同一種異常在恢復正常前只寄一次。

## 控制狀態機

控制狀態機負責所有致動決策。澆水不是「低於門檻就一直開水泵」，而是補一段時間後等待滲透，再用新的讀值重新判斷，用來處理土壤濕度的延遲：水澆下去後，要一段時間才會被感測器讀到。土壤太濕時進入風乾，濕度回到正常範圍才關風扇。

```mermaid
stateDiagram-v2
    state "正常運作" as NORMAL {
        state "IDLE 待機" as IDLE
        IDLE : 每圈看最新濕度做判斷
        state "WATERING 澆水" as WATERING
        WATERING : 水泵運轉
        state "SOAKING 等待滲透" as SOAKING
        SOAKING : 不做決定，讀值照常記錄
        state "DRYING 風乾" as DRYING
        DRYING : 風扇運轉

        [*] --> IDLE
        IDLE --> WATERING : 太乾 且 水箱有水
        IDLE --> DRYING : 土壤太濕
        WATERING --> SOAKING : 澆水時間到 或 途中水箱低水位（關水泵）
        SOAKING --> IDLE : 等待時間到
        DRYING --> IDLE : 濕度回到正常（關風扇）
    }
    state "FAULT 感測器異常" as FAULT
    FAULT : 水泵、風扇保持關閉

    [*] --> NORMAL
    NORMAL --> FAULT : 讀值異常（關水泵與風扇）
    FAULT --> NORMAL : 讀值恢復正常（回到 IDLE）
```

- 狀態機同一時間只會在一個狀態，所以澆水和吹風不會同時發生。
- 澆到一半水箱低水位時，也進入 SOAKING：已經澆進去的水還沒滲到感測器，先等滲透再判斷，避免補完水箱後馬上又多澆一次。
- 讀值異常時（例如感測器的線鬆脫，讀到不合理的數值），不論目前在哪個狀態都進入 FAULT，水泵和風扇全部關閉；讀值恢復正常後回到 IDLE 重新判斷。「正常運作」大框把四個狀態包在一起，所以只畫一條線，就代表四個狀態都這樣處理。框內的黑點表示每次進入「正常運作」都從 IDLE 開始。
- 「讀值異常」的判斷標準由土壤感測模組定義，`readSoilMoisture()` 回傳 -1 代表異常。

## 模組與負責人

| 模組 | 檔案 | 負責人 | 主要函式 |
|---|---|---|---|
| 主程式 | `SmartPlanter.ino` | 建瑄 | `setup()`、`loop()` |
| 土壤感測 | `soil_sensor.cpp` | 淯瑋 | `readSoilMoisture()` |
| 控制邏輯 | `control.cpp` | 明杰 | `controlUpdate()` |
| 致動器 | `actuators.cpp` | 玟君 | `pumpOn()`、`fanOn()`、`buzzerOn()` |
| 水位與安全 | `safety.cpp` | 娟華 | `checkWaterSafety()`、`isLowWater()` |
| 通知 | `notify.cpp` | 承儒 | `sendAlert()`、`notifyUpdate()` |
