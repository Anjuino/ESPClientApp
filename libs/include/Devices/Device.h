#pragma once
#include <EEPROM.h>
#include <WebSocketsClient.h>
#include "OTA/OTA.h"
#include "ArduinoJson.h"
#include "Config/build.h"

#ifdef ESP32
  #include <WiFi.h>
  #include <WebServer.h>
#else
  #include <ESP8266WiFi.h>
  #include "ESP8266WebServer.h"
#endif

class Device {
   
   private:
      WebSocketsClient Client;

      size_t totalWritten = 0;
      bool isFirstPacket = true;
      MD5Builder md5;

      #ifdef ESP32
        WebServer *server = NULL;
      #else
        ESP8266WebServer *server = NULL;
      #endif

      void WebSocketInit();
      void HandlerWebSocket(WStype_t type, uint8_t* payload, size_t length);
      
      uint32_t GetChipId() 
      {
      #ifdef ESP32 
         return ESP.getEfuseMac();
      #else 
         return ESP.getChipId();
      #endif  
      }

      struct DeviceSmartHomeSetting{
        bool SettingIsEmpty;
        char Token[20];
        char ServerIp[32];
      };

    protected:
      DeviceSmartHomeSetting Settings;

   public:
      uint16_t SettingsAddress = 0;

      #ifdef ESP32

        Device (uint16_t SettingsAddress, WebServer *server)  
        {
          this->SettingsAddress = SettingsAddress;
          this->server = server;
        };

      #else
        Device (uint16_t SettingsAddress, ESP8266WebServer *server)  
        {
          this->SettingsAddress = SettingsAddress;
          this->server = server;
        };
      #endif

      ~Device() {};

      void Init();

      void virtual Loop();
      void virtual AppStart() = 0;

      void virtual ParseIncomingMessage(JsonDocument doc, String TypeMesseage);
      void SendMesseageToServer(JsonDocument doc);
      void SendLog(String Log);
      void UpdateFirmware(uint8_t* data, size_t len);
      void RequestFirmware();
      void RequestNextFirmwarePacket();
      void SendOtaFinish();
      void ResetOtaUpdate();
      void SendSetting();
      void WriteSetting(JsonDocument doc);
};

