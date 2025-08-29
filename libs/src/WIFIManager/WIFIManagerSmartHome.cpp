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
    Serial.println(DeviceSmartHomeSetting.ServerIp);

}

void WIFIManagerSmartHome::Start(bool IsNeedEEPROM)
{
    WIFIManager::Start(IsNeedEEPROM);   // Базовое подключение к WIFI сети

    if (strlen(DeviceSmartHomeSetting.Token) != 0) return;
    else {                         
        Serial.println("Запускаю точку доступа");
        WiFi.softAP("ESP", "password");

        StartServer();
        
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
    String Ip = server.arg("Ip");

    Serial.println(Token);
    Serial.println(Ip);

    if (Token.length() < sizeof(DeviceSmartHomeSetting.Token) && 
        Ip.length() < sizeof(DeviceSmartHomeSetting.ServerIp)) {
        
        if (Token.length() != 0) Token.toCharArray(DeviceSmartHomeSetting.Token, sizeof(DeviceSmartHomeSetting.Token));
        if (Ip.length() != 0) Ip.toCharArray(DeviceSmartHomeSetting.ServerIp, sizeof(DeviceSmartHomeSetting.ServerIp));

        EEPROM.put(SmartDeviceSettingAddress, DeviceSmartHomeSetting);
        EEPROM.commit();
        
        Serial.println(DeviceSmartHomeSetting.Token);
        Serial.println(DeviceSmartHomeSetting.ServerIp);

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

void WIFIManagerSmartHome::GetIpServer()
{
    String JSONAnswer = "{";
    JSONAnswer += "\"Ip\":\"" + String(DeviceSmartHomeSetting.ServerIp) + "\"}";

    server.send(200, "application/json", JSONAnswer);
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

    server.on("/GetIpServer", [this]() {GetIpServer();});   
    server.on("/GetToken", [this]() {GetToken();});     
}
