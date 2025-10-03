#pragma once
#if defined ESP8266 && defined TELEMETRY_CONTROLLER

#include "Device.h"
#include "Sensors/SensorBase.h"
#include <vector>

namespace SensorTypes {
    constexpr const char* Temperature = "Temperature";
    constexpr const char* Humidity    = "Humidity";
    constexpr const char* CO2         = "CO2ppm";
}

#define AddressZone1 120       // Адрес настроек настроек для eeprom

class DeviceTelemetry: public Device {
   
   private:
        struct SensorInfo {
                SensorBase* Sensor;
                String Type;     
        };

        std::vector<SensorInfo> Sensors;
    
        void AppStart() override;
        
   public:

        void AddSensor(SensorBase* Sensor, const String& Type);

        DeviceTelemetry(uint16_t SettingsAddress, ESP8266WebServer *server) : Device(SettingsAddress, server) {}
        ~DeviceTelemetry() {};

        void Init();

        void Loop() override;
        
        void ParseIncomingMessage(JsonDocument doc, String TypeMesseage) override;

        void SendState();

};

#endif