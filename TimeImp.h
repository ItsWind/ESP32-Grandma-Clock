#pragma once

#include <Arduino.h>

namespace TimeImp {
  unsigned long GetRTCEpoch();
  void Init();
  void OnSleep();
  bool ConnectWiFi();
  void DisconnectWiFi();
  unsigned long GetLastSavedEpoch();
  void AddToSyncTimer(unsigned long dt);
  void SyncNTP();
  int GetHour();
  int GetMinute();
  String GetAMPM();
}