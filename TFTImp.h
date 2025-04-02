#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

namespace TFTImp {
  extern TFT_eSPI Screen;

  void Init();
  void Update(unsigned long dt);
  void ResetScreenDimTimer();
  void SetClockScreen();
  void SetChangingTexts();
  void SetTime();
  void SetHour(uint8_t hour);
  void SetMinute(uint8_t minute);
  void SetAMPM(String amPm);
  void SetTemp(int16_t temp);
  void SetHumidity(uint8_t humidity);
}