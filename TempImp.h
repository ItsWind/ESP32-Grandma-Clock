#pragma once

#include <Arduino.h>

namespace TempImp {
  void Init();
  void Update(unsigned long dt);
  void DoRead();
  float GetTemp();
  float GetHumidity();
}