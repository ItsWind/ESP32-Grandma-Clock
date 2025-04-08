#include "SoundImp.h"
#include "Constants.h"
#include "TimeImp.h"
#include "TempImp.h"
#include "TFTImp.h"

#include <AudioTools.h>
#include <TalkiePCM.h>
//#include <Vocab_US_Clock.h>
#include <Vocab_US_Large.h>

static const uint16_t SOUND_CHANNELS = 2;
static const sample_rate_t SOUND_SAMPLE_RATE = 8000;
static const uint8_t SOUND_BITS_PER_SAMPLE = 16;
static const float TONE_AMPLITUDE = 4000;
static const float VOICE_VOLUME = 1.0;

static I2SStream out;

static TalkiePCM voice(out, SOUND_CHANNELS);

static SineFromTable<int16_t> sineWave(TONE_AMPLITUDE);
static GeneratedSoundStream sineStream(sineWave);
static StreamCopy sineCopier(out, sineStream, 32);

static bool toneCue = false;
static unsigned long toneTimer = 0;

static uint8_t lastSetLEDPWM = 0;
static void setLEDPWM(uint8_t pwm) {
  if (lastSetLEDPWM == pwm) {
    return;
  }

  ledcWrite(BLUE_LED_PIN, pwm);
  lastSetLEDPWM = pwm;
  //delay(1);
}

namespace SoundImp {
  void Init() {
    auto cfg = out.defaultConfig();
    cfg.i2s_format = I2S_STD_FORMAT;
    cfg.sample_rate = SOUND_SAMPLE_RATE;
    cfg.channels = SOUND_CHANNELS;
    cfg.bits_per_sample = SOUND_BITS_PER_SAMPLE;

    // i2s pins used on ESP32
    cfg.pin_bck = SPEAKER_BCLK_PIN;
    cfg.pin_ws = SPEAKER_WSLRC_PIN;
    cfg.pin_data = SPEAKER_DOUT_PIN;

    out.begin(cfg);
    delay(50);

    voice.setVolume(VOICE_VOLUME);
  }

  void Update(unsigned long dt) {
    if (sineWave.isActive()) {
      sineCopier.copy();
    }

    if (toneCue) {
      toneTimer += dt;

      if (toneTimer >= 1000000) {
        toneTimer = 0;
        toneCue = false;
        sineWave.end();
        setLEDPWM(0);

        SayTime();
        voice.sayPause();
        SayTemp();
      }
      else {
        float lerpVal = (float)toneTimer / 1000000.0;
        
        float pwmVal = round(lerp(255.0, 0.0, lerpVal));
        if (pwmVal < 0.0) {
          pwmVal = 0.0;
        }
        
        float ampVal = lerp(TONE_AMPLITUDE, 0.0, lerpVal);
        if (ampVal < 0.0) {
          ampVal = 0.0;
        }
        
        setLEDPWM((uint8_t)pwmVal);
        sineWave.setAmplitude(ampVal);
      }
    }
  }

  void SayTime() {
    int currHour = TimeImp::GetHour();
    int currMinute = TimeImp::GetMinute();
    String currAmPm = TimeImp::GetAMPM();

    voice.say(sp3_THE);
    voice.say(sp3_TIME);
    voice.say(sp3_IS);

    voice.sayNumber((long)currHour);
    //int timeMinute = TimeImp::GetMinute();
    if (currMinute > 0) {
      if (currMinute < 10) {
        voice.say(sp2_O);
      }
      voice.sayNumber((long)currMinute);
    }

    //String amPm = TimeImp::GetAMPM();
    if (currAmPm.equals("AM")) {
      voice.say(sp3_A_M);
    }
    else {
      voice.say(sp3_P_M);
    }
  }

  void SayTemp() {
    voice.sayNumber((long)round(TempImp::GetTemp()));

    voice.say(sp3_DEGREES);
    voice.say(sp3_FARENHEIT);
  }

  bool DoingFullReport() {
    return toneCue;
  }

  void SayFullReport() {
    toneCue = true;
    sineWave.setAmplitude(TONE_AMPLITUDE);
    sineWave.begin((int)SOUND_CHANNELS, (int)SOUND_SAMPLE_RATE, 650);

    TFTImp::ResetScreenDimTimer();
    setLEDPWM(255);
  }
}