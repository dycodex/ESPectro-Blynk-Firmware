#include <FS.h>
#include <ESP8266WiFi.h>

#include "AppConfig.h"

AppConfig::AppConfig() {
  defaultConfigFile = (char*) malloc(sizeof(char) * 100);
  internalConfigBufferSize = sizeof(config_t);
  internalConfigBuffer = (uint8_t*) malloc(internalConfigBufferSize);
  storedConfig = (config_t*) malloc(sizeof(config_t));

  memset(defaultConfigFile, 0, 100);
  memset(internalConfigBuffer, 0, internalConfigBufferSize);
  memset(storedConfig, 0, sizeof(config_t));
  strcpy(defaultConfigFile, DEFAULT_CONFIG_FILENAME);
}

AppConfig::~AppConfig() {
  free(defaultConfigFile);
  free(internalConfigBuffer);
  free(storedConfig);
}

void AppConfig::loadFile() {
  const char* filename = defaultConfigFile;

  if (!SPIFFS.exists(filename)) {
    Serial.println("[ERROR] Config file is not found");
    return;
  }

  File configFile = SPIFFS.open(filename, "r");

  if (!configFile) {
    Serial.println("[ERROR] Failed to open config file");
    return;
  }

  // while(!configFile.available());
  size_t avail = configFile.available();

  if (!avail) {
    Serial.printf("[ERROR] Buffer size mismatch. %d, %d\n", avail, internalConfigBufferSize);
    return;
  }

  size_t size = configFile.read(internalConfigBuffer, internalConfigBufferSize);
  if (size < 1) {
    Serial.printf("[ERROR] Got size %d\n", size);
    return;
  }

  storedConfig = (config_t*) internalConfigBuffer;
  Serial.printf("[READ] Blynk token: %s\n", storedConfig->blynkToken);
  Serial.printf("[READ] SSID: %s\n", storedConfig->ssid);
  Serial.printf("[READ] Password: %s\n", storedConfig->password);
}

void AppConfig::begin() {
  if (!SPIFFS.begin()) {
    Serial.println("[ERROR] AppConfig failed to initialize SPIFFS!");
    return;
  }

#if APPCONFIG_RESET_WIFI
  wifiManager.resetSettings();
  SPIFFS.remove(defaultConfigFile);
#endif

  loadFile();

  WiFiManagerParameter blynkTokenParam("blynk", "Blynk Token", storedConfig->blynkToken, 34);
  wifiManager.addParameter(&blynkTokenParam);
  wifiManager.setTimeout(120);
  wifiManager.setAPCallback(configModeCallback);

#if APPCONFIG_RESET_WIFI == 0
  if (storedConfig != nullptr && strlen(storedConfig->ssid) > 0 && strlen(storedConfig->password) > 0) {
    delay(1000);
    return;
  }
#endif

  wifiManager.setBreakAfterConfig(true);
  if (!wifiManager.autoConnect(DEFAULT_ACCESSPOINT_NAME)) {
    Serial.println("[ERROR] Failed to set access point and blynk token! Resetting...");
    delay(500);

    ESP.reset();
  }

  strcpy(storedConfig->blynkToken, blynkTokenParam.getValue());
  strcpy(storedConfig->ssid, WiFi.SSID().c_str());
  strcpy(storedConfig->password, WiFi.psk().c_str());

  Serial.printf("[SAVE] Blynk token: %s\n", storedConfig->blynkToken);
  Serial.printf("[SAVE] SSID: %s\n", storedConfig->ssid);
  Serial.printf("[SAVE] Password: %s\n", storedConfig->password);

  delay(1000);
  saveConfig();
}

void AppConfig::setConfigFilename(char* filename) {
  if (filename == nullptr) {
    return;
  }

  defaultConfigFile = filename;
}

void AppConfig::saveConfig() {
  File configFile = SPIFFS.open(defaultConfigFile, "w");
  if (!configFile) {
    Serial.println("[ERROR] AppConfig failed to open config file");
    return;
  }

  internalConfigBuffer = (uint8_t*)storedConfig;
  internalConfigBufferSize = sizeof(config_t);

  Serial.printf("Gonna write %d bytes\n", internalConfigBufferSize);

  configFile.write((uint8_t*)storedConfig, internalConfigBufferSize);
  configFile.close();
}

void AppConfig::reset() {

}

config_t* AppConfig::getStoredConfig() {
  return storedConfig;
}

void AppConfig::setOnEnteredConfigModeCallback(WiFiManagerEnteredConfigCallback callback) {
  configModeCallback = callback;
}
