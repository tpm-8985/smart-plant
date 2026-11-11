#pragma once
// 【固定介面】setup 呼叫一次；loop 每圈呼叫 update。
void wifiInit();
void wifiUpdate();
bool wifiIsConnected();
