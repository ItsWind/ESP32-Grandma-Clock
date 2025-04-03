#pragma once

#include <Arduino.h>

namespace TimeImp {
  void Init();
  void OnSleep();
  bool ConnectWiFi();
  void DisconnectWiFi();
  void AddToSyncTimer(unsigned long dt);
  void SyncNTP();
  int GetHour();
  int GetMinute();
  String GetAMPM();
}