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
    static bool isFirstPacket = true;
    static MD5Builder md5;

    if (isFirstPacket) {
        Serial.println("OTA начата");
        md5.begin();
        isFirstPacket = false;
        totalWritten = 0;
        
        #if defined(ESP8266)
            size_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace)) {
        #else
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        #endif
            Serial.println("Ошибка при запуске процесса OTA");
            SendLog("Ошибка при запуске процесса OTA");
            isFirstPacket = true;
            return;
        }
    }

    // Записываем данные
    if (Update.write(data, len) != len) {
        Serial.println("Ошибка при записи");
        SendLog("Ошибка при записи");
        Update.end(false);
        isFirstPacket = true;
        return;
    }

    if (len < 4096) { 
        size_t dataSize = len - 32;
        
        if (dataSize > 0) md5.add(data, dataSize);
        
        md5.calculate();
        String calculated_md5 = md5.toString();
        
        String expected_md5;
        for (int i = 0; i < 32; i++) {
            expected_md5 += (char)data[dataSize + i];
        }

        //Serial.printf("Ожидаемый MD5: %s\n", expected_md5.c_str());
        //Serial.printf("Вычисленный MD5: %s\n", calculated_md5.c_str());
        
        if (calculated_md5 != expected_md5) {
            Serial.println("MD5 не совпал");
            SendLog("MD5 не совпал");
            Update.end(false);
        } else {
            //Serial.println("MD5 совпал");
            if (Update.end(true)) {
                //Serial.println("OTA успешна");
                SendLog("OTA успешна");
                delay(2000);
                ESP.restart();
            } else {
                SendLog("Ошибка при финализации");
                Serial.println("Ошибка при финализации");
            }
        }
        
        isFirstPacket = true;
        totalWritten = 0;
    } else {
        md5.add(data, len);
        totalWritten += len;
        //Serial.printf("Принято: %d байт, Всего: %d байт\n", len, totalWritten);
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