#include "TempImp.h"
#include "Constants.h"

#include <AM2302-Sensor.h>

static AM2302::AM2302_Sensor dht{TEMP_SENSOR_PIN};
static unsigned long dhtReadTimer = 0;

namespace TempImp {
  void Init() {
    dht.begin();
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
  }

  float GetTemp() {
    float celcius = dht.get_Temperature();
    // return farenheit
    return (celcius * 9 / 5) + 32;
  }

  float GetHumidity() {
    return dht.get_Humidity();
  }
}