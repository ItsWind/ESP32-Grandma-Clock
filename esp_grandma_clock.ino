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
  digitalWrite(SCREEN_DIM_PIN, HIGH);
  gpio_hold_dis((gpio_num_t)SCREEN_DIM_PIN);

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

  TimeImp::Init();
  
  if (SleepImp::WasSleeping) {
    esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();
    switch (wakeupCause) {
      case ESP_SLEEP_WAKEUP_TIMER: {
        TimeImp::AddToSyncTimer(1);

        TempImp::StartComm();
        delay(2000);
        TempImp::DoRead();

        SleepImp::SetToSleep();
        return;
      }
      case ESP_SLEEP_WAKEUP_EXT0: {
        break;
      }
    }
  }

  TempImp::Init();

  TFTImp::Init();

  SoundImp::Init();

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

  // Check button press
  if (SleepImp::WasSleeping) {
    SleepImp::WasSleeping = false;

    buttonPressed = true;
    SoundImp::SayFullReport();
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
