#pragma once

#include <Arduino.h>

const uint64_t TEMP_CHECK_SLEEP_TIMER = 120000000;
const uint64_t TIME_SYNC_TIMER_MAX = 3600000000;

const uint8_t SPEAKER_DOUT_PIN = 33;
const uint8_t SPEAKER_BCLK_PIN = 14;
const uint8_t SPEAKER_WSLRC_PIN = 26;

const uint8_t TEMP_SENSOR_PIN = 27;

const uint8_t SCREEN_DIM_PIN = 4;

const uint8_t BLUE_LED_PIN = 2;

const uint8_t BUTTON_PIN = 32;
const uint16_t BUTTON_DEBOUNCE_MICROS = 20000;