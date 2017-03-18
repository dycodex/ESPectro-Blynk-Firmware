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

  File configFile = SPIFFS.open(filename, "r+");

  if (!configFile) {
    Serial.println("[ERROR] Failed to open config file");
    return;
  }

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

  configFile.close();
}

void AppConfig::begin() {
  if (!SPIFFS.begin()) {
    Serial.println("[ERROR] AppConfig failed to initialize SPIFFS!");
    return;
  }

  loadFile();
}

void AppConfig::saveConfig(const char* ssid, const char* password, const char* blynkToken) {
  strcpy(storedConfig->ssid, ssid);
  strcpy(storedConfig->password, password);
  strcpy(storedConfig->blynkToken, blynkToken);

  save();
}

void AppConfig::save() {
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
  if (SPIFFS.exists(defaultConfigFile)) {
    SPIFFS.remove(defaultConfigFile);
  }

  memset(storedConfig->blynkToken, 0, sizeof(storedConfig->blynkToken));
  memset(storedConfig->ssid, 0, sizeof(storedConfig->ssid));
  memset(storedConfig->password, 0, sizeof(storedConfig->password));

  save();
}

config_t* AppConfig::getStoredConfig() {
  return storedConfig;
}

bool AppConfig::isNotConfigured() {
  return strlen(storedConfig->ssid) < 1
    && strlen(storedConfig->password) < 1
    && strlen(storedConfig->blynkToken) < 1;
}
