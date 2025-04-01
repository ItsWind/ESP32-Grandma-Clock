#include "TimeImp.h"
#include "secrets.h"
#include "Constants.h"
#include "SoundImp.h"

#include <WiFi.h>
//#include <WiFiUdp.h>
#include <AsyncUDP.h>
//#include <NTPClient.h>
#include <ESP32Time.h>

static ESP32Time rtc(0); // -18000
//static WiFiUDP ntpUDP;
//static NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
static AsyncUDP foolTimeUDP;
static bool syncUDPInProgress = false;
static uint64_t syncTimer = 0;

namespace TimeImp {
  void Init() {
    SyncNTP();
  }

  bool ConnectWiFi() {
    setCpuFrequencyMhz(240);

    WiFi.mode(WIFI_STA);
    WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASS);
    uint8_t tries = 0;
    while (WiFi.waitForConnectResult(1000) != WL_CONNECTED && tries < 4) {
      Serial.println("Retrying wifi connect");
      WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASS);
      tries++;
    }

    return WiFi.status() == WL_CONNECTED;
  }

  void DisconnectWiFi() {
    WiFi.mode(WIFI_OFF);
    WiFi.disconnect();
    
    setCpuFrequencyMhz(80);
  }

  void AddToSyncTimer(unsigned long dt) {
    syncTimer += (uint64_t)dt;
    if (syncTimer >= TIME_SYNC_TIMER_MAX) {
      Serial.println("Start RTC sync");
      SyncNTP();
    }
  }

  void SyncNTP() {
    if (SoundImp::DoingFullReport()) {
      return;
    }

    syncTimer = 0;
    Serial.println("Set sync timer to 0");

    if (WiFi.status() != WL_CONNECTED) {
      if (!ConnectWiFi()) {
        // TO-DO: If wifi connection failed
        DisconnectWiFi();
        Serial.println("Wifi failed");
        return;
      }
    }
    //timeClient.begin();
    Serial.println("Wifi connected");

    if (foolTimeUDP.connect(IPAddress(54,39,21,229), 40556)) {
      Serial.println("UDP connection made");
      syncUDPInProgress = true;
      foolTimeUDP.onPacket([&](AsyncUDPPacket packet) {
        // Receive packet
        const uint8_t * bytes = packet.data();
        uint32_t epochReceived = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
        rtc.setTime(epochReceived);
        syncUDPInProgress = false;
      });

      uint8_t tries = 0;
      while (syncUDPInProgress && tries < 10) {
        // Send request packet
        Serial.println("Sending UDP try");
        const uint8_t requestBytes[] = {1};
        foolTimeUDP.write(requestBytes, 1);
        tries++;
        delay(100);
      }

      if (syncUDPInProgress) {
        Serial.println("Sync failed on request");
        syncUDPInProgress = false;
      }
      else {
        Serial.println("Sync retrieved");
      }

      foolTimeUDP.close();
    }
    else {
      Serial.println("UDP failed");
    }

    /*bool rtcSyncSuccess = timeClient.forceUpdate();
    uint8_t tries = 0;
    while (!rtcSyncSuccess && tries < 255) {
      rtcSyncSuccess = timeClient.forceUpdate();
      tries++;
    }

    if (rtcSyncSuccess) {
      Serial.println("time client updated");
      
      rtc.setTime(timeClient.getEpochTime());
      Serial.println("RTC synced");
    }
    else {
      Serial.println("RTC sync failed");
    }

    timeClient.end();*/
    
    DisconnectWiFi();
    Serial.println("Wifi disconnected");
  }

  int GetHour() {
    return rtc.getHour();
  }

  int GetMinute() {
    return rtc.getMinute();
  }

  String GetAMPM() {
    return rtc.getAmPm();
  }
}