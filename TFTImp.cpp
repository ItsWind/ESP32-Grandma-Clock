#include "TFTImp.h"
#include "Constants.h"
#include "TempImp.h"
#include "TimeImp.h"
#include "SleepImp.h"

static const unsigned long SCREEN_FADE_TIME = 30000000;

static uint8_t lastSetHour = 255;
static uint8_t lastSetMinute = 255;
static uint8_t lastSetAMPM = 255;
static int16_t lastSetTemp = 1000;
static uint8_t lastSetHumidity = 255;

static unsigned long screenSetTextsTimer = 0;
static unsigned long screenDimTimer = 0;
static uint8_t lastSetScreenDimPWM = 0;
static void setScreenDim(uint8_t pwm) {
  if (lastSetScreenDimPWM == pwm) {
    return;
  }

  ledcWrite(SCREEN_DIM_PIN, pwm);
  lastSetScreenDimPWM = pwm;
  //delay(1);
}

static void DrawCenteredText(uint8_t size, int16_t x, int16_t y, int32_t bgWidthOffset, int32_t bgHeightOffset, const char * text) {
  TFTImp::Screen.setTextSize(size);

  int16_t textWidth = TFTImp::Screen.textWidth(text);
  int16_t textHeight = TFTImp::Screen.fontHeight();
  int16_t x1 = x - (textWidth / 2);
  int16_t y1 = y - (textHeight / 2);

  TFTImp::Screen.fillRect(x1, y1, (int32_t)textWidth + bgWidthOffset, (int32_t)textHeight + bgHeightOffset, TFT_BLUE);

  TFTImp::Screen.setCursor(x1, y1);
  TFTImp::Screen.print(text);

  TFTImp::Screen.setTextSize(1);
}
static void DrawCenteredText(uint8_t size, int16_t x, int16_t y, const char * text) {
  DrawCenteredText(size, x, y, 0, 0, text);
}

namespace TFTImp {
  bool WasSleeping = false;
  TFT_eSPI Screen = TFT_eSPI();

  void Init() {
    Screen.init();
    Screen.setRotation(1);
    Screen.fillScreen(TFT_BLACK);
    Screen.setTextColor(TFT_WHITE);
    Screen.setTextWrap(false);
    Screen.setCursor(0, 0);

    Screen.println("yo");
  }

  void Update(unsigned long dt) {
    screenSetTextsTimer += dt;
    if (screenSetTextsTimer >= 500000) {
      screenSetTextsTimer = 0;
      SetChangingTexts();
    }

    if (screenDimTimer < SCREEN_FADE_TIME) {
      screenDimTimer += dt;
      if (screenDimTimer >= SCREEN_FADE_TIME) {
        setScreenDim(255);
        //screenDimTimer = 15000000;
        screenDimTimer = 0;
        
        SleepImp::SetToSleep();
      }
      else {
        float screenDimLerpVal = (float)screenDimTimer / (float)SCREEN_FADE_TIME;
        setScreenDim((uint8_t)round(lerp(0.0, 254.0, screenDimLerpVal)));
      }
    }
  }

  void ResetScreenDimTimer() {
    screenDimTimer = 0;
  }

  void SetClockScreen() {
    Screen.fillScreen(TFT_BLUE);

    auto halfHeight = Screen.height() / 2;
    auto halfWidth = Screen.width() / 2;

    DrawCenteredText(7, 160, halfHeight - 60, ":");

    Screen.drawFastHLine(20, halfHeight-1, 280, TFT_WHITE);
    Screen.drawFastHLine(20, halfHeight, 280, TFT_WHITE);
    Screen.drawFastVLine(halfWidth-1, halfHeight + 20, 80, TFT_WHITE);
    Screen.drawFastVLine(halfWidth, halfHeight + 20, 80, TFT_WHITE);
  }

  void SetChangingTexts() {
    SetTime();
    SetTemp((int16_t)round(TempImp::GetTemp()));
    SetHumidity((uint8_t)round(TempImp::GetHumidity()));
  }

  void SetTime() {
    SetHour((uint8_t)TimeImp::GetHour());
    SetMinute((uint8_t)TimeImp::GetMinute());
    SetAMPM(TimeImp::GetAMPM());
  }

  void SetHour(uint8_t hour) {
    if (hour == lastSetHour) {
      return;
    }

    String hourStr = String(hour);
    if (hourStr.length() == 1) {
      hourStr = "0" + hourStr;
    }

    auto halfHeight = Screen.height() / 2;
    //Screen.fillRect(50, halfHeight - 50, 90, 90, TFT_BLUE);
    DrawCenteredText(7, 100, halfHeight - 60, -1, 0, hourStr.c_str());

    lastSetHour = hour;

    Serial.println("Set hour");
  }

  void SetMinute(uint8_t minute) {
    if (minute == lastSetMinute) {
      return;
    }

    String minuteStr = String(minute);
    if (minuteStr.length() == 1) {
      minuteStr = "0" + minuteStr;
    }

    auto halfHeight = Screen.height() / 2;
    //Screen.fillRect(170, halfHeight - 50, 90, 90, TFT_BLUE);
    DrawCenteredText(7, 220, halfHeight - 60, -1, 0, minuteStr.c_str());

    lastSetMinute = minute;

    Serial.println("Set minute");
  }

  void SetAMPM(String amPm) {
    bool shouldSet = false;
    if (amPm.equals("PM") && lastSetAMPM != 0) {
      shouldSet = true;
      lastSetAMPM = 0;

      Serial.println("Set PM");
    }
    else if (amPm.equals("AM") && lastSetAMPM != 1) {
      shouldSet = true;
      lastSetAMPM = 1;

      Serial.println("Set AM");
    }

    if (shouldSet) {
      auto halfHeight = Screen.height() / 2;
      //Screen.fillRect(260, halfHeight - 20 - 16, 50, 40, TFT_BLUE);
      DrawCenteredText(4, 285, halfHeight - 76, amPm.c_str());
    }
  }

  void SetTemp(int16_t temp) {
    if (temp == lastSetTemp) {
      return;
    }

    String tempStr = String(temp) + "F";

    auto halfHeight = Screen.height() / 2;
    //Screen.fillRect(170, halfHeight - 50, 90, 90, TFT_BLUE);
    DrawCenteredText(5, 75, halfHeight + 60, tempStr.c_str());

    lastSetTemp = temp;

    Serial.println("Set temp");
  }

  void SetHumidity(uint8_t humidity) {
    if (humidity == lastSetHumidity) {
      return;
    }

    String humidityStr = String(humidity) + "%";

    auto halfHeight = Screen.height() / 2;
    //Screen.fillRect(170, halfHeight - 50, 90, 90, TFT_BLUE);
    DrawCenteredText(5, 245, halfHeight + 60, humidityStr.c_str());

    lastSetHumidity = humidity;

    Serial.println("Set humidity");
  }
}