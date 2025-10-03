#if defined ESP32 && defined LED_CONTROLLER
#pragma once
#include "Device.h"
#include "Led/WS2812.h"

#define AddressSettings 120       // Адрес настроек настроек для eeprom
#define SensorDetectedMove false

class DeviceLedController: public Device {
   
   private:
      class WS2812 Led;
      void AppStart() override;
      void SetState(JsonDocument doc);
      void SetSpeed(JsonDocument doc);
      void SetBrightness(JsonDocument doc);
      void UpdateSettingLed(JsonDocument doc);
      void SendLedSetting();

      static void LedCode(void *pvParameters);

   public:

      DeviceLedController(uint16_t SettingsAddress, WebServer *server) : Device(SettingsAddress, server) {}
      ~DeviceLedController() {};

      void Init();

      void Loop() override;
      
      void ParseIncomingMessage(JsonDocument doc, String TypeMesseage) override;

      void SendState();

};

#endif