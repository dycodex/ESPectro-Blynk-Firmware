#ifndef BLYNK_APPCONFIG_H
#define BLYNK_APPCONFIG_H

#include <WiFiManager.h>

#define DEFAULT_CONFIG_FILENAME "/app_config.dat"
#define DEFAULT_ACCESSPOINT_NAME "EspectroWifi-1"

typedef void (*WiFiManagerShouldSaveCallback)();
typedef void (*WiFiManagerEnteredConfigCallback)(WiFiManager *wifiManager);

typedef struct {
  char blynkToken[34];
  char ssid[100];
  char password[16];
} config_t;

class AppConfig {
public:
  AppConfig();
  ~AppConfig();
  void begin();
  void setConfigFilename(char* filename);
  void saveConfig();
  void setOnEnteredConfigModeCallback(WiFiManagerEnteredConfigCallback callback);
  void reset();

  config_t* getStoredConfig();
private:
  WiFiManager wifiManager;
  char* defaultConfigFile;
  config_t* storedConfig;
  uint8_t* internalConfigBuffer;
  size_t internalConfigBufferSize;

  void loadFile();
  WiFiManagerShouldSaveCallback shouldSaveCallback;
  WiFiManagerEnteredConfigCallback configModeCallback;
};

#endif