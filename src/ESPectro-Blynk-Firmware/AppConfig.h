#ifndef BLYNK_APPCONFIG_H
#define BLYNK_APPCONFIG_H

#include <WiFiManager.h>

#define DEFAULT_CONFIG_FILENAME "/app_config.dat"

typedef struct {
  char blynkToken[34];
  char ssid[64];
  char password[64];
} config_t;

class AppConfig {
public:
  AppConfig();
  ~AppConfig();
  void begin();
  void reset();
  void saveConfig(const char* ssid, const char* password, const char* blynkToken);
  bool isNotConfigured();

  config_t* getStoredConfig();
private:
  char* defaultConfigFile;
  config_t* storedConfig;
  uint8_t* internalConfigBuffer;
  size_t internalConfigBufferSize;

  void loadFile();
  void save();
};

#endif