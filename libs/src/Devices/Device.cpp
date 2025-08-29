#include "Devices/Device.h"

void Device::SendLog(String Log)
{   
    JsonDocument doc;

    // Заполняем объект данными
    doc["TypeMesseage"] = "Log";
    doc["Log"]          = Log;

    SendMesseageToServer(doc);
}

void Device::SendMesseageToServer(JsonDocument doc)
{   
    doc["ChipId"]       = GetChipId();

    String jsonString;
    serializeJson(doc, jsonString);

    //Serial.println(jsonString);
    Client.sendTXT(jsonString);
}

void Device::RequestFirmware()
{
    JsonDocument doc;

    // Заполняем объект данными
    char Token[20];
    EEPROM.get(SettingsAddress, Token);
    
    doc["Token"]        = Token;
    doc["TypeMesseage"] = "GetFirmware";

    SendMesseageToServer(doc);
}

void Device::ParseIncomingMessage(JsonDocument doc, String TypeMesseage)
{
    if(TypeMesseage == "StartOTA") RequestFirmware();
    if(TypeMesseage == "Reboot")   ESP.restart();
}

void Device::UpdateFirmware(uint8_t* data, size_t len) {
    static size_t totalWritten = 0;

    if (totalWritten == 0) {
        //Serial.println("Начинаем OTA...");
        #if defined(ESP8266)
            size_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace)) {
        #else
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  // Для ESP32
        #endif
            //Serial.println("Не удалось начать OTA!");
            return;
        }
    }

    // Запись данных
    size_t written = Update.write(data, len);
    if (written != len) {
        //Serial.printf("Ошибка записи! %d/%d байт\n", written, len);
        Update.end(false);  // Аварийное завершение
        totalWritten = 0;
        return;
    }
    totalWritten += written;

    //Serial.printf("Записано: %d байт (всего: %d)\n", written, totalWritten);

    if (len < 4096) {
        if (Update.end(true)) {
            delay(10);
            ESP.restart();
        } 
        else Serial.println("Ошибка финализации: " + String(Update.getError()));
        totalWritten = 0;
    }
}

void Device::Init()
{   
    WebSocketInit();
}

void Device::Loop()
{
    Client.loop();
}

void Device::HandlerWebSocket(WStype_t type, uint8_t* payload, size_t length)
{
    switch(type) {
        case WStype_CONNECTED:
        {
            Serial.println("Подключился к серверу");
            delay(100);
            AppStart();
            break;
        }

        case WStype_DISCONNECTED:
        { 
            Serial.println("Отключился от сервера");
            break;
        }

        case WStype_PING:
        {
            //Serial.println("Пинг");
            break;
        }

        case WStype_TEXT:
        {
            JsonDocument doc;
            deserializeJson(doc, payload);
            String TypeMesseage = doc["TypeMesseage"];

            ParseIncomingMessage(doc, TypeMesseage);
            break;
        }

        case WStype_BIN: 
        { 
            UpdateFirmware(payload, length);
            break;
        }
    }
}

void Device::WebSocketInit()
{
    struct{
        bool SettingIsEmpty;
        char Token[20];
        char ServerIp[15];
    } DeviceSmartHomeSetting;

    EEPROM.get(SettingsAddress, DeviceSmartHomeSetting);

    Client.begin(DeviceSmartHomeSetting.ServerIp, PORT, Gate);

    Client.onEvent([this](WStype_t type, uint8_t* payload, size_t length) {
        this->HandlerWebSocket(type, payload, length);  // Вызов метода класса
    });

    Client.setAuthorization(USER, SECRET_KEY);

    Client.setReconnectInterval(10000);
}