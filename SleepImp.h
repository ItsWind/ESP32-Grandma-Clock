#pragma once

#include <Arduino.h>

namespace SleepImp {
  extern RTC_DATA_ATTR bool WasSleeping;

  void SetToSleep();
}