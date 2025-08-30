#pragma once
#ifdef ESP8266

#include "Device.h"
#include "Sensors/SensorBase.h"
#include <vector>

namespace SensorTypes {
    constexpr const char* Temperature = "Temperature";
    constexpr const char* Humidity    = "Humidity";
    constexpr const char* CO2         = "CO2ppm";
}

#define AddressZone1 120       // Адрес настроек настроек для eeprom
#define AddressZone2 150
#define AddressZone3 180
#define AddressZone4 210
#define AddressZone5 240


class DeviceTelemetry: public Device {
   
   private:
        struct SensorInfo {
                SensorBase* Sensor;
                String Type;     
                String Location;  
        };

        std::vector<SensorInfo> Sensors;

        void UpdateZoneName(JsonDocument doc);

        struct ZoneInfo { 
                char Location[29];
                bool IsEmpty;  
        };
        
        void AppStart() override;
   public:

        void AddSensor(SensorBase* Sensor, const String& Type, uint16_t AddressZone);

        DeviceTelemetry(uint16_t SettingsAddress, ESP8266WebServer *server) : Device(SettingsAddress, server) {}
        ~DeviceTelemetry() {};

        void Init();

        void Loop() override;
        
        void ParseIncomingMessage(JsonDocument doc, String TypeMesseage) override;

        void SendState();

};

#endif