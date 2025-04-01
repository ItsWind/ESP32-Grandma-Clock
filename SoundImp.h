#pragma once

#include <Arduino.h>

namespace SoundImp {
  void Init();
  void Update(unsigned long dt);
  void SayTime();
  void SayTemp();
  bool DoingFullReport();
  void SayFullReport();
}