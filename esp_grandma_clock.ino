#include <Arduino.h>
#include "Constants.h"
#include "TFTImp.h"
#include "SoundImp.h"
#include "TempImp.h"
#include "TimeImp.h"
#include "SleepImp.h"

static bool buttonPressed = false;
static unsigned long buttonDebounceTimer = 0;

static void initPins() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  pinMode(SCREEN_DIM_PIN, OUTPUT);
  ledcAttachChannel(SCREEN_DIM_PIN, 490, 8, 15);
  ledcWrite(SCREEN_DIM_PIN, 0);

  pinMode(2, OUTPUT);
  ledcAttachChannel(2, 490, 8, 13);
  ledcWrite(2, 0);

  pinMode(BLUE_LED_PIN, OUTPUT);
  ledcAttachChannel(BLUE_LED_PIN, 490, 8, 14);
  ledcWrite(BLUE_LED_PIN, 0);
}

unsigned long oldTime = 0;
void setup() {
  initPins();

  Serial.begin(115200);
  //delay(1000);
  
  if (SleepImp::WasSleeping) {
    esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();
    switch (wakeupCause) {
      case ESP_SLEEP_WAKEUP_TIMER: {
        TempImp::DoRead();
        SleepImp::SetToSleep();
        return;
      }
      case ESP_SLEEP_WAKEUP_EXT0: {
        break;
      }
    }
  }

  TFTImp::Init();

  //TFTImp::Screen.println("Init time");
  TimeImp::Init();
  
  //TFTImp::Screen.println("Init temp");
  TempImp::Init();

  //TFTImp::Screen.println("Init sound");
  SoundImp::Init();

  //TFTImp::Screen.println("Setting clock screen");

  TFTImp::SetClockScreen();

  oldTime = micros();
}

void loop() {
  unsigned long thisTime = micros();
  unsigned long dt = 0;
  // Handle overflow
  if (thisTime < oldTime) {
    unsigned long maxNum = 0;
    maxNum--;

    dt = (maxNum - oldTime) + thisTime + 1;
  }
  else {
    dt = thisTime - oldTime;
  }
  oldTime = thisTime;

  //Serial.print("DT: ");
  //Serial.println(dt);

  /*uint8_t wakeUpStatus = SleepImp::CheckWakeUpTime();
  switch (wakeUpStatus) {
    case 1: {
      dt = 0;
      break;
    }
    case 2: {
      // Skip rest of loop if wake up is due to wakeup timer
      // Wakeup timer will set to sleep again immediately after
      // When waking again, it will return here to return; and start loop
      return;
    }
  }*/

  // Check button press
  if (SleepImp::WasSleeping) {
    buttonPressed = true;
    SoundImp::SayFullReport();
    SleepImp::WasSleeping = false;
  }
  else if (digitalRead(BUTTON_PIN) == LOW && !SoundImp::DoingFullReport()) {
    if (!buttonPressed) {
      buttonDebounceTimer += dt;
      if (buttonDebounceTimer >= BUTTON_DEBOUNCE_MICROS) {
        buttonPressed = true;
        SoundImp::SayFullReport();
      }
    }
  }
  else {
    buttonPressed = false;
    buttonDebounceTimer = 0;
  }
  
  TimeImp::AddToSyncTimer(dt);

  SoundImp::Update(dt);

  TempImp::Update(dt);

  TFTImp::Update(dt);
}
