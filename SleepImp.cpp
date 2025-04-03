#include "SleepImp.h"
#include "Constants.h"
#include "TFTImp.h"
#include "SoundImp.h"
#include "TempImp.h"
#include "TimeImp.h"

namespace SleepImp {
  RTC_DATA_ATTR bool WasSleeping = false;

  void SetToSleep() {
    WasSleeping = true;

    TFTImp::Screen.sleep(true);
    
    TimeImp::OnSleep();

    gpio_hold_en((gpio_num_t)SCREEN_DIM_PIN);
    esp_sleep_enable_timer_wakeup(TEMP_CHECK_SLEEP_TIMER);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);
    esp_deep_sleep_start();
  }
}