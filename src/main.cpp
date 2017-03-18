#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <ESPectro.h>

#include "AppConfig.h"
#include "NeoPixelData.h"

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

#define DEFAULT_ACCESSPOINT_NAME "ESPectroCoreWiFi-000"
#define DEFAULT_ACCESSPOINT_PASS "11223344"

Ticker ticker;
WiFiManager manager;
ESPectro board(ESPectro_V3);
ESPectro_Button button(ESPectro_V3, ESPECTRO_BUTTON_PIN_V3);
AppConfig appConfig;
NeoPixelData neoPixelData;

void setNeopixelColor(uint32_t color) {
  for (int i = 0; i < 3; i++) {
    board.turnOnNeopixel(HtmlColor(color), i);
  }
}

void onTimerTick() {
  board.toggleLED();
}

void configModeCallback(WiFiManager *wifiManager) {
  ticker.attach(0.2, onTimerTick);
}


void onButtonLongPressed() {
  Serial.println("Gonna reset all configuration!");
  delay(1000);

  appConfig.reset();
  ESP.reset();
}

void setup() {
  board.turnOffLED();
  board.turnOffAllNeopixel();

  delay(3000);
  Serial.begin(9600);

  button.onLongPressed(onButtonLongPressed);
  button.begin();

  // Uncomment line below only if you know what you're doing or else this will be your undoing
  // SPIFFS.format();
  appConfig.begin();

  manager.setAPCallback(configModeCallback);
  manager.setConnectTimeout(30);

  Serial.println("Initializing...");
  WiFiManagerParameter blynkTokenParam("blynk", "Blynk Token", appConfig.getStoredConfig()->blynkToken, 34);
  manager.addParameter(&blynkTokenParam);
  manager.setTimeout(120);
  manager.setBreakAfterConfig(true);

  if (appConfig.isNotConfigured()) {
    if (!manager.startConfigPortal(DEFAULT_ACCESSPOINT_NAME, DEFAULT_ACCESSPOINT_PASS)) {
      Serial.println("[ERROR] Failed to configure connection");
      delay(500);

      ESP.reset();
    }
  } else {
    if (!manager.autoConnect(DEFAULT_ACCESSPOINT_NAME, DEFAULT_ACCESSPOINT_PASS)) {
      Serial.println("[ERROR] Failed to configure connection");
      delay(500);

      ESP.reset();
    }
  }

  delay(500);

  appConfig.saveConfig(WiFi.SSID().c_str(), WiFi.psk().c_str(), blynkTokenParam.getValue());
  ticker.detach();
  board.turnOffLED();

  Serial.println("Setup is done...");
  Serial.printf("AP: %s\n", appConfig.getStoredConfig()->ssid);
  Serial.printf("PSK: %s\n", appConfig.getStoredConfig()->password);

  Blynk.config(appConfig.getStoredConfig()->blynkToken,
    "cloud.makestro.com",
    8442
  );

  Blynk.connect();
}

void loop() {
  button.run();
  Blynk.run();
}

// NeoPixel's power
BLYNK_WRITE(V0) {
  int value = param.asInt();
  Serial.printf("Got value from V0: %d\n", value);

  if (value) {
    for (int i = 0; i < 3; i++) {
      board.turnOnNeopixel(HtmlColor(neoPixelData.getColor()), i);
    }

    neoPixelData.setState(value);
  } else {
    board.turnOffAllNeopixel();
    neoPixelData.setState(value);
  }
}

// NeoPixel's red color value
BLYNK_WRITE(V1) {
  int value = param.asInt();
  Serial.printf("Got Red Value: %d\n", value);

  neoPixelData.setRed(value);
  if (neoPixelData.getState()) {
    setNeopixelColor(neoPixelData.getColor());
  }
}

BLYNK_WRITE(V2) {
  int value = param.asInt();
  Serial.printf("Got Green Value: %d\n", value);

  neoPixelData.setGreen(value);
  if (neoPixelData.getState()) {
    setNeopixelColor(neoPixelData.getColor());
  }
}

BLYNK_WRITE(V3) {
  int value = param.asInt();
  Serial.printf("Got Blue Value: %d\n", value);

  neoPixelData.setBlue(value);
  if (neoPixelData.getState()) {
    setNeopixelColor(neoPixelData.getColor());
  }
}
