#if defined ESP8266 && defined TELEMETRY_CONTROLLER

#include "Devices/DeviceTelemetry.h"

#include "Sensors/ENS160Sensor.h"
#include "Sensors/BME280Sensor.h"
#include "Sensors/AHTSensor.h"
#include "Sensors/DHTSensor.h"

void DeviceTelemetry::ParseIncomingMessage(JsonDocument doc, String TypeMesseage)
{
    Device::ParseIncomingMessage(doc, TypeMesseage);

    if(TypeMesseage == "GetState") SendState();

    // Тут специфика разобра сообщений именно для контороллера телеметрии
}

void DeviceTelemetry::AppStart()
{
    JsonDocument doc;

    // Заполняем объект данными
    doc["TypeMesseage"] = "Authentication";
    doc["Token"]        = Settings.Token;
    doc["TypeDevice"]   = "Telemetry";
    doc["Build"]        = build;

    SendMesseageToServer(doc);
}

void DeviceTelemetry::Init()
{
    Device::Init();

    //AddSensor(static_cast<SensorT*>(new BME280Sensor()),    SensorTypes::Temperature);
    //AddSensor(static_cast<SensorH*>(new BME280Sensor()),    SensorTypes::Humidity);
    AddSensor(static_cast<SensorT*>(new DHTSensor()),    SensorTypes::Temperature);
    AddSensor(static_cast<SensorH*>(new DHTSensor()),    SensorTypes::Humidity);
   // AddSensor(static_cast<SensorCO2*>(new ENS160Sensor()),  SensorTypes::CO2);
}


void DeviceTelemetry::AddSensor(SensorBase* Sensor, const String& Type)
{
    Sensor->Init();
    delay(10);
    Sensors.push_back({Sensor, Type});
}


void DeviceTelemetry::SendState(bool ToDataBase)
{
    JsonDocument doc;

    if(ToDataBase) doc["TypeMesseage"] = "ToDataBase"; 
    else           doc["TypeMesseage"] = "State";

    JsonObject zoneObj;
    JsonArray ZonesArray = doc.createNestedArray("Data");
    zoneObj = ZonesArray.createNestedObject();

    // Добавляем данные датчика
    for (const auto& SensorInfo : Sensors) {
        auto value = SensorInfo.Sensor->GetData();

        if (SensorInfo.Type == "Temperature")    zoneObj[SensorInfo.Type] = std::get<float>(value); 
        else if (SensorInfo.Type == "Humidity")  zoneObj[SensorInfo.Type] = std::get<float>(value);  
        else if (SensorInfo.Type == "CO2ppm")    zoneObj[SensorInfo.Type] = std::get<uint16_t>(value);

        delay(50);
    }

    SendMesseageToServer(doc);
}

void DeviceTelemetry::Loop()
{
    Device::Loop();

    if(Timer < millis()) {
        Timer = millis() + 60 * 60 * 1000;
        SendState(true);
    }
}

#endif