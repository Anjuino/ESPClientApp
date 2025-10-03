#include "WIFIManager/WIFIManagerSmartHome.h"

WIFIManagerSmartHome::WIFIManagerSmartHome()
{

}

void WIFIManagerSmartHome::Init(bool IsNeedEEPROM)
{
    WIFIManager::Init(IsNeedEEPROM);    // Чтение настроек WIFI из eeprom

    EEPROM.get (SmartDeviceSettingAddress, DeviceSmartHomeSetting); // Чтение настроек из eeprom

    if (DeviceSmartHomeSetting.SettingIsEmpty) {
        memset(&DeviceSmartHomeSetting, 0, sizeof(DeviceSmartHomeSetting));
        EEPROM.put(SmartDeviceSettingAddress, DeviceSmartHomeSetting);
        EEPROM.commit();
    }

    Serial.println(DeviceSmartHomeSetting.Token);
}

void WIFIManagerSmartHome::Start(bool IsNeedEEPROM)
{
    WIFIManager::Start(IsNeedEEPROM);   // Базовое подключение к WIFI сети

    StartServer();
    
    if (strlen(DeviceSmartHomeSetting.Token) != 0) return;
    else {                         
        Serial.println("Запускаю точку доступа");
        
        #ifdef ESP32
            const char* ApName = "ESP32";
        #endif

        #ifdef ESP8266
            const char* ApName = "ESP8266";
        #endif

        WiFi.softAP(ApName, "password");

        while(true) {
            server.handleClient ();
            delay(0);
        }
    }
}

void WIFIManagerSmartHome::RegDeviceData(bool IsNeedRestart)
{
    WIFIManager::RegDeviceData(false);

    String Token = server.arg ("Token");
    Serial.println(Token);

    if (Token.length() < sizeof(DeviceSmartHomeSetting.Token)) {
        
        if (Token.length() != 0) Token.toCharArray(DeviceSmartHomeSetting.Token, sizeof(DeviceSmartHomeSetting.Token));

        EEPROM.put(SmartDeviceSettingAddress, DeviceSmartHomeSetting);
        EEPROM.commit();
        
        Serial.println(DeviceSmartHomeSetting.Token);

        delay(100);
        if (IsNeedRestart) ESP.restart();
    } 
    else server.send(503, "text/plane", "Fail");

    if (IsNeedRestart) ESP.restart();
}

void WIFIManagerSmartHome::ResetSettings()
{
    for (int i = 0; i < sizeof(DeviceSmartHomeSetting); i++) { 
        EEPROM.write((SmartDeviceSettingAddress + i), 0x00);
    }
    EEPROM.commit();
}

void WIFIManagerSmartHome::GetToken()
{
    String JSONAnswer = "{";
    JSONAnswer += "\"Token\":\"" + String(DeviceSmartHomeSetting.Token) + "\"}";

    server.send(200, "application/json", JSONAnswer);
}

void WIFIManagerSmartHome::StartServer()
{
    WIFIManager::StartServer();

    server.on("/GetToken", [this]() {GetToken();});     
}
