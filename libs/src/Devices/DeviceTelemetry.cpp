#ifdef ESP8266

#include "Devices/DeviceTelemetry.h"

#include "Sensors/ENS160Sensor.h"
#include "Sensors/AHTSensor.h"
#include "Sensors/TestSensor.h"
#include "Sensors/DS18B20.h"

void DeviceTelemetry::ParseIncomingMessage(JsonDocument doc, String TypeMesseage)
{
    Device::ParseIncomingMessage(doc, TypeMesseage);

    if(TypeMesseage == "GetState") {
        //SendLog("Запрос состояния");
        SendState();
    }
    
    if(TypeMesseage == "UpdateZoneName") {
        SendLog("Обновляю имена зоны");
        UpdateZoneName(doc);
    }
    // Тут специфика разобра сообщений именно для контороллера телеметрии
}

void DeviceTelemetry::AppStart()
{
    JsonDocument doc;

    struct{
        bool SettingIsEmpty;
        char Token[20];
        char ServerIp[15];
    } DeviceSmartHomeSetting;

    EEPROM.get(SettingsAddress, DeviceSmartHomeSetting);

    // Заполняем объект данными
    doc["TypeMesseage"] = "Authentication";
    doc["Token"]        = DeviceSmartHomeSetting.Token;
    doc["TypeDevice"]   = "Telemetry";
    doc["Build"]        = build;

    SendMesseageToServer(doc);
}

void DeviceTelemetry::Init()
{
    Device::Init();

    AddSensor(static_cast<SensorT*>(new AHTSensor()),      SensorTypes::Temperature, AddressZone1);
    AddSensor(static_cast<SensorH*>(new AHTSensor()),      SensorTypes::Humidity, AddressZone1);
    AddSensor(static_cast<SensorCO2*>(new ENS160Sensor()), SensorTypes::CO2, AddressZone1);

    //AddSensor(static_cast<SensorT*>(new DS18B20()),      SensorTypes::Temperature, AddressZone2);
    /*AddSensor(static_cast<SensorT*>(new TestSensor()),      SensorTypes::Temperature, AddressZone1);
    AddSensor(static_cast<SensorH*>(new TestSensor()),      SensorTypes::Humidity,    AddressZone1);
    AddSensor(static_cast<SensorCO2*>(new TestSensor()),    SensorTypes::CO2,         AddressZone1);*/
}

void DeviceTelemetry::AddSensor(SensorBase* Sensor, const String& Type, uint16_t AddressZone)
{
    Sensor->Init();

    ZoneInfo ZoneInfo;

    EEPROM.get(AddressZone, ZoneInfo);

    if (ZoneInfo.IsEmpty) {
        if (AddressZone == AddressZone1) strncpy(ZoneInfo.Location, "Зона 1", sizeof(ZoneInfo.Location));
        if (AddressZone == AddressZone2) strncpy(ZoneInfo.Location, "Зона 2", sizeof(ZoneInfo.Location));
        if (AddressZone == AddressZone3) strncpy(ZoneInfo.Location, "Зона 3", sizeof(ZoneInfo.Location));
        if (AddressZone == AddressZone4) strncpy(ZoneInfo.Location, "Зона 4", sizeof(ZoneInfo.Location));
        if (AddressZone == AddressZone5) strncpy(ZoneInfo.Location, "Зона 5", sizeof(ZoneInfo.Location));

        Serial.println("Делаю перезапись");

        ZoneInfo.IsEmpty = false;
        EEPROM.put(AddressZone, ZoneInfo);

        EEPROM.commit();
    }

    Serial.println(ZoneInfo.Location);
    delay(10);
    Sensors.push_back({Sensor, Type, ZoneInfo.Location});
}


void DeviceTelemetry::UpdateZoneName(JsonDocument doc)
{
    uint8_t NumZone = doc["NumZone"];
    String OldName  = doc["OldName"];
    String NewName  = doc["NewName"];

    for (auto& SensorInfo : Sensors) {
        if (SensorInfo.Location == OldName) {
            SensorInfo.Location = NewName;

            uint16_t AddressZone;

            if (NumZone == 1) AddressZone = AddressZone1; 
            if (NumZone == 2) AddressZone = AddressZone2; 
            if (NumZone == 3) AddressZone = AddressZone3; 
            if (NumZone == 4) AddressZone = AddressZone4;
            if (NumZone == 5) AddressZone = AddressZone5; 

            Serial.println("Делаю перезапись");

            ZoneInfo ZoneInfo;
            EEPROM.get(AddressZone, ZoneInfo);

            String StoredName = ZoneInfo.Location;

            if (StoredName == OldName) {
                strncpy(ZoneInfo.Location, NewName.c_str(), sizeof(ZoneInfo.Location));
                
                EEPROM.put(AddressZone, ZoneInfo);
            }
        }
        EEPROM.commit();
        delay(1);
    }

    JsonDocument docAnswer;

    docAnswer["TypeMesseage"] = "UpdateZoneName";
    SendMesseageToServer(docAnswer);

}

void DeviceTelemetry::SendState()
{
    JsonDocument doc;
    doc["TypeMesseage"] = "State";

    // Создаем массив для хранения данных по зонам
    JsonArray locationArray = doc.createNestedArray("Location");

    for (const auto& SensorInfo : Sensors) {
        auto value = SensorInfo.Sensor->GetData();

        // Ищем объект зоны в массиве
        JsonObject zoneObj;
        for (JsonObject obj : locationArray) {
            if (obj["Zone"] == SensorInfo.Location) {
                zoneObj = obj;
                break;
            }
        }

        // Если не нашли - создаем новую зону
        if (zoneObj.isNull()) {
            zoneObj = locationArray.createNestedObject();
            zoneObj["Zone"] = SensorInfo.Location;
        }

        // Добавляем данные датчика
        if (SensorInfo.Type == "Temperature" || SensorInfo.Type == "Humidity") zoneObj[SensorInfo.Type] = std::get<float>(value);  
        else if (SensorInfo.Type == "CO2ppm")                                  zoneObj[SensorInfo.Type] = std::get<uint16_t>(value);   

    }

    SendMesseageToServer(doc);
}

void DeviceTelemetry::Loop()
{
    Device::Loop();
}

#endif