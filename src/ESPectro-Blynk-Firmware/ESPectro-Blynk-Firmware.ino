#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <ESPectro.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

#include "AppConfig.h"
#include "NeoPixelData.h"

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

#define DEFAULT_ACCESSPOINT_NAME "ESPectroCoreWiFi-000"
#define DEFAULT_ACCESSPOINT_PASS "12345678"

#define CONNECTIO_PIN 10
#define USE_BMP true

Adafruit_BMP085 bmpSensor;
// Ticker ticker;
Ticker analogReadingTicker;
WiFiManager manager;
ESPectro board(ESPectro_V3);
ESPectro_Button button(ESPectro_V3, ESPECTRO_BUTTON_PIN_V3);
AppConfig appConfig;
NeoPixelData neoPixelData;

int connectioThreshold = 0;

void setNeopixelColor(uint32_t color) {
  for (int i = 0; i < 3; i++) {
    board.turnOnNeopixel(HtmlColor(color), i);
  }
}

void onAnalogSensorTriggered() {
  int reading = analogRead(A0);

  if (reading < connectioThreshold) {
    digitalWrite(CONNECTIO_PIN, HIGH);
  }
}

void onTickerTick() {
  board.toggleLED();
}

void configModeCallback(WiFiManager *wifiManager) {
  // ticker.attach(0.2, onTickerTick);
  board.fadeLED(800);
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

  if (USE_BMP) {
    if (!bmpSensor.begin()) {
      Serial.println("Could not initialize BMP Sensor. Please make sure that you have BMP sensor connected to the I2C pins");
      while (1) {}
    }
  }

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
    manager.resetSettings();

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
  // ticker.detach();
  board.stopLEDAnimation();
  board.turnOffLED();
  analogReadingTicker.attach(0.1, onAnalogSensorTriggered);

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

// NeoPixel's green color
BLYNK_WRITE(V2) {
  int value = param.asInt();
  Serial.printf("Got Green Value: %d\n", value);

  neoPixelData.setGreen(value);
  if (neoPixelData.getState()) {
    setNeopixelColor(neoPixelData.getColor());
  }
}

// NeoPixel's blue color
BLYNK_WRITE(V3) {
  int value = param.asInt();
  Serial.printf("Got Blue Value: %d\n", value);

  neoPixelData.setBlue(value);
  if (neoPixelData.getState()) {
    setNeopixelColor(neoPixelData.getColor());
  }
}

// BMP Sensor Temperature
BLYNK_READ(V4) {
  if (USE_BMP) {
    Blynk.virtualWrite(V4, bmpSensor.readTemperature());
  } else {
    Blynk.virtualWrite(V4, 0);
  }
}

// BMP Sensor Pressure
BLYNK_READ(V5) {
  if (USE_BMP) {
    Blynk.virtualWrite(V5, bmpSensor.readPressure());
  } else {
    Blynk.virtualWrite(V5, 0);
  }
}

// BMP Sensor Altitude
BLYNK_READ(V6) {
  if (USE_BMP) {
    Blynk.virtualWrite(V6, bmpSensor.readAltitude());
  } else {
    Blynk.virtualWrite(V6, 0);
  }
}

// BMP Sensor Sea-level Pressure
BLYNK_READ(V7) {
  if (USE_BMP) {
    Blynk.virtualWrite(V7, bmpSensor.readSealevelPressure());
  } else {
    Blynk.virtualWrite(V7, 0);
  }
}

BLYNK_WRITE(V8) {
  connectioThreshold = param.asInt();
}
