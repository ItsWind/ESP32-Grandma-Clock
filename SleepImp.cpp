#include "SleepImp.h"
#include "Constants.h"
#include "TFTImp.h"
#include "SoundImp.h"
#include "TempImp.h"
#include "TimeImp.h"

static bool wasSleeping = false;

namespace SleepImp {
  void SetToSleep() {
    wasSleeping = true;
    TFTImp::Screen.sleep(true);

    esp_sleep_enable_timer_wakeup(15000000);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);
    esp_light_sleep_start();
  }

  uint8_t CheckWakeUpTime() {
    if (!wasSleeping) {
      return 0;
    }
    
    esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();
    switch (wakeupCause) {
      case ESP_SLEEP_WAKEUP_TIMER: {
        TempImp::DoRead();
        SetToSleep();
        return 2;
      }
      case ESP_SLEEP_WAKEUP_EXT0: {
        TFTImp::Screen.sleep(false);
        TempImp::DoRead();
        TFTImp::SetChangingTexts();
        SoundImp::SayFullReport();
        break;
      }
    }

    wasSleeping = false;

    return 1;
  }
}