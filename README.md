# 智能澆水盆栽 Smart Watering Planter

國立臺灣師範大學 嵌入式系統設計（CSC0027）第 1 組期末專題。

系統感測盆栽土壤濕度，過乾時依缺水程度自動補水並等待滲透後重新量測，過濕時啟動風扇加速乾燥；水箱低水位時強制停泵，並以蜂鳴器與 Email 通知使用者。

## 資料夾結構

```
smart-watering-planter/
├── firmware/SmartPlanter/   Arduino 程式（用 Arduino IDE 開啟 SmartPlanter.ino）
│   ├── SmartPlanter.ino     主程式（建瑄）
│   ├── interfaces.h         全組共用的函式介面（建瑄）
│   ├── config.h             腳位、校正值、控制參數（各項標註負責人）
│   ├── soil_sensor.cpp      土壤感測（淯瑋）
│   ├── control.cpp          控制狀態機（明杰）
│   ├── actuators.cpp        水泵、風扇、蜂鳴器（玟君）
│   ├── safety.cpp           水位與安全保護（娟華）
│   ├── notify.cpp           Wi-Fi 與 Email（承儒）
│   └── secrets.example.h    Wi-Fi 與寄信資料的範本
├── docs/main-flow.md        主迴圈流程圖與控制狀態機
└── data/                    實驗數據
```

## 分組

| 開發板 | 成員 | 負責 |
|---|---|---|
| Board A：UNO R4 WiFi | 建瑄、承儒 | 主程式、系統整合、Wi-Fi、Email |
| Board B | 淯瑋、明杰 | 土壤感測、校正、控制狀態機 |
| Board C | 玟君、娟華 | 水泵、繼電器、風扇、水位、蜂鳴器、安全保護 |

最終成品只有一塊 **Arduino UNO R4 WiFi**，所有模組合成同一個程式，在這塊板子上執行。Board B、C 只是讓各組在整合前能同時測試自己的部分；測試時接線也請照 `config.h` 的腳位，整合到同一塊板子時才不用改程式，也不會有兩個模組搶同一支腳。

## 目前的狀態：骨架

每個檔案只寫好函式名稱、負責人，以及每個函式要做到的功能與流程

- 每個檔案開頭寫著這個檔案的工作、會用到的常數、以及依進度規劃表的時程。
- 每個函式上方寫著誰會呼叫它、要做什麼、流程與注意事項。
- 有回傳值的函式先放了一個暫時的 `return`，只是為了讓專案能編譯，實作時刪掉。
- 在 Arduino IDE 按 Ctrl+Shift+F（Mac 是 Cmd+Shift+F）搜尋 `TODO(你的名字)`，就能找到自己要寫的地方。
- `config.h` 列出了大家共用的常數名稱，數值由負責人決定後，把那一行的註解取消再填入。
- 整體流程與每個狀態的轉換條件，以 `docs/main-flow.md` 為準。

## 開始使用

1. 用 Arduino IDE 開啟 `firmware/SmartPlanter/SmartPlanter.ino`，同資料夾的其他檔案會自動出現在分頁上。
2. 選擇開發板：Board A B C 選 **Arduino UNO R4 WiFi**（第一次使用要先在開發板管理員安裝 Arduino UNO R4 Boards）
3. 按「驗證」確認可以編譯成功。

### Wi-Fi 與 Email

把 `secrets.example.h` 複製一份，改名為 `secrets.h`，填入 Wi-Fi 與寄信資料。**`secrets.h` 已列在 `.gitignore`，絕對不要把密碼寫進其他會上傳的檔案。** 沒有 `secrets.h` 的組員也必須能編譯，這點由承儒在 `notify.cpp` 處理。

## 協作規則

1. **每次開始工作前先 Pull**，拿到其他人最新的程式。
2. **每週開一個新分支做事**，名稱用英文小寫加連字號，例如 `soil-calibration`、`email-alert`。不要直接改 `main`。
3. **只改自己負責的檔案。** 需要動到別人的檔案，先跟對方說。
4. **`interfaces.h` 只在週三會議討論後修改**，因為改了它，其他人的程式也要跟著改。
5. **`config.h` 是共用的**，改腳位或參數前先在群組說一聲。
6. 做完就 Commit 並 Push，到 GitHub 上開 Pull Request。**每週三由建瑄合併進 `main`**，合併前要能編譯成功。
7. Commit 訊息寫清楚做了什麼，例如「新增土壤感測器乾濕端點校正」，不要只寫「更新」。
8. 實作完成的部分，記得把 `TODO` 和暫時的 `return` 刪掉。
