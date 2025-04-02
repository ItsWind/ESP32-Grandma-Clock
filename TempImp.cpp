#include "TempImp.h"
#include "Constants.h"
#include "SleepImp.h"

#include <AM2302-Sensor.h>

static AM2302::AM2302_Sensor dht{TEMP_SENSOR_PIN};
static unsigned long dhtReadTimer = 0;

RTC_DATA_ATTR float savedTemp = 0;
RTC_DATA_ATTR float savedHumidity = 0;

namespace TempImp {
  void Init() {
    if (!SleepImp::WasSleeping) {
      dht.begin();
    }
  }

  void Update(unsigned long dt) {
    dhtReadTimer += dt;
    if (dhtReadTimer >= 5000000) {

      DoRead();
    }
  }

  void DoRead() {
    dhtReadTimer = 0;

    auto result = dht.read();
    uint8_t tries = 0;
    while (result != 0 && tries < 255) {
      result = dht.read();
      tries++;
    }

    if (result == 0) {
      Serial.println("temp saved");
      savedTemp = (dht.get_Temperature() * 9 / 5) + 32;
      savedHumidity = dht.get_Humidity();
    }
  }

  float GetTemp() {
    return savedTemp;
    /*float celcius = dht.get_Temperature();
    // return farenheit
    return (celcius * 9 / 5) + 32;*/
  }

  float GetHumidity() {
    return savedHumidity;
    //return dht.get_Humidity();
  }
}