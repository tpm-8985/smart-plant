#pragma once

// 【可更改】Wi-Fi 設定
#define WIFI_SSID      "islab-5G"
#define WIFI_PASSWORD  "postquantumcryptography"

// 【可更改／必填】Google 16 字元應用程式密碼（移除顯示用空格）。不是登入密碼或 Resend key。
// ** 這個請別動！ **
#define GOOGLE_APP_PASSWORD "euqe cwkj ebmx wgna"

// 【可更改】收件者：目前寄給自己
// 嗨，組員，這個測試時可以設成你自己的
#define ALERT_EMAIL_TO "61547121s@gapps.ntnu.edu.tw"

// 【固定】依本專案要求，寄件者與 SMTP 登入帳號相同
// 嗨組員，這個請固定，測試時不需要我(楊承儒)這邊即時的操作才能發送mail，謝謝
#define ALERT_EMAIL_FROM "61547121s@gapps.ntnu.edu.tw"

// 【固定】Google SMTP，465 為直接 TLS；不可直接換成 587
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
