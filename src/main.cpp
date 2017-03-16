#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include "AppConfig.h"

#define BLYNK_PRINT Serial
#define APPCONFIG_RESET_WIFI 0
#include <BlynkSimpleEsp8266.h>

Ticker ticker;

void onTimerTick() {
  int state = digitalRead(LED_BUILTIN);
  digitalWrite(LED_BUILTIN, !state);
}

void configModeCallback(WiFiManager *wifiManager) {
  ticker.attach(0.2, onTimerTick);
}

void setup() {
  Serial.begin(9600);
  delay(3000);
  pinMode(LED_BUILTIN, OUTPUT);

  AppConfig appConfig;

  Serial.println("Initializing...");

  appConfig.setOnEnteredConfigModeCallback(configModeCallback);
  appConfig.begin();
  Serial.println("Setup is done...");
  Serial.printf("AP: %s\n", appConfig.getStoredConfig()->ssid);
  Serial.printf("PSK: %s\n", appConfig.getStoredConfig()->password);

  WiFi.disconnect();
  ticker.detach();
  digitalWrite(LED_BUILTIN, LOW);

  Blynk.begin(appConfig.getStoredConfig()->blynkToken,
    appConfig.getStoredConfig()->ssid,
    appConfig.getStoredConfig()->password,
    "cloud.makestro.com",
    8442
  );
}

void loop() {
  Blynk.run();
}
