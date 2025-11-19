#ifdef ESP32
  #include <WiFi.h>
  #include <WebServer.h>
#else
  #include <ESP8266WiFi.h>
  #include "ESP8266WebServer.h"
#endif

#include <EEPROM.h>

#include "webPage/Index.h"
#include "webPage/css.h"

#define WifiSettingAddress 0                // Адрес настроек для eeprom

class WIFIManager {

    private:

    protected:
      struct{
          bool SettingIsEmpty;
          char wifiSSID[32];
          char wifiPassword[16];
      } DeviceSetting;

      #ifdef ESP32
        WebServer server;
      #else
        ESP8266WebServer server;
      #endif
      
      virtual void StartServer();
      void GetCSS();
      void StartPage();
      virtual void RegDeviceData(bool IsNeedRestart = true);
      virtual void Init(bool IsNeedEEPROM);
      virtual void Start(bool IsNeedEEPROM);

    public:

      WIFIManager();
      ~WIFIManager() {};

      void ResetWifiSetting();

      void ScanWifi();
      void GetWifiData();

      #ifdef ESP32
        WebServer *GetServer(void) {return &server;};
      #else
        ESP8266WebServer *GetServer(void) {return &server;};
      #endif  
};