#include "WIFIManager/WIFIManager.h"

WIFIManager::WIFIManager()
{

}

void WIFIManager::Init(bool IsNeedEEPROM)
{
    if (IsNeedEEPROM) EEPROM.begin (512);

    delay(10);
    
    EEPROM.get (WifiSettingAddress, DeviceSetting);

    if (DeviceSetting.SettingIsEmpty) {
        memset(&DeviceSetting, 0, sizeof(DeviceSetting));
        EEPROM.put(WifiSettingAddress, DeviceSetting);
        EEPROM.commit();
    }

    Serial.println(DeviceSetting.wifiSSID);
    Serial.println(DeviceSetting.wifiPassword);
}

void WIFIManager::ResetWifiSetting()
{
    for (int i = 0; i < sizeof(DeviceSetting); i++) { 
        EEPROM.write(i, 0x00);
    }
    EEPROM.commit();
}

void WIFIManager::Start(bool IsNeedEEPROM)
{   
    Init(IsNeedEEPROM);

    WiFi.mode(WIFI_STA);     // Включаю режим станиции

    WiFi.begin(DeviceSetting.wifiSSID, DeviceSetting.wifiPassword);   // Попытка подключения к сети

    uint64_t TimeConnect = millis() + 6000;
    bool IsConnect = true;
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
      if (millis() > TimeConnect) {
        IsConnect = false;
        break;
      }
    }

    // Не смог подключится к сети. Создаю точку доступа и запускаю сервер
    if (!IsConnect) {
        Serial.println("Запускаю точку доступа");
        
        #ifdef ESP32
            const char* ApName = "ESP32";
        #endif

        #ifdef ESP8266
            const char* ApName = "ESP8266";
        #endif

        WiFi.softAP(ApName, "password");

        StartServer();
        
        while(true) {
            server.handleClient ();
            delay(0);
        }
    }
}


void WIFIManager::GetCSS()
{
    server.send (200, "text/css", css);
}

void WIFIManager::StartPage()
{
    server.send (200, "text/html", Index);
}

void WIFIManager::GetWifiData()
{
    String JSONAnswer = "{";
    JSONAnswer += "\"SSID\":\"" + String(DeviceSetting.wifiSSID) + "\" ,";
    JSONAnswer += "\"Password\":\"" + String(DeviceSetting.wifiPassword) + "\"}";

    server.send(200, "application/json", JSONAnswer);
}

void WIFIManager::EspRestart()
{
    server.send(200, "text/plain", "OK");
    delay(1000);
    ESP.restart();
}

void WIFIManager::ScanWifi()
{
    int netCount = WiFi.scanNetworks();
    String JSONAnswer = "["; 

    for (int i = 0; i < netCount; i++) {
        JSONAnswer += "{"; 

        JSONAnswer += "\"SSID\":\"" + String(WiFi.SSID(i)) + "\",";
        JSONAnswer += "\"RSSI\":\"" + String(WiFi.RSSI(i)) + "\"";

        JSONAnswer += "}"; 

        if (i < netCount - 1) JSONAnswer += ",";
    }

    JSONAnswer += "]"; 

    server.send(200, "application/json", JSONAnswer);
}

void WIFIManager::RegDeviceData(bool IsNeedRestart) 
{
  String SSID = server.arg ("SSID");
  String Password = server.arg ("Password");

  if (SSID.length() < sizeof(DeviceSetting.wifiSSID) && Password.length() < sizeof(DeviceSetting.wifiPassword)) {
    if (SSID.length() != 0)      SSID.toCharArray(DeviceSetting.wifiSSID, sizeof(DeviceSetting.wifiSSID));
    if (Password.length() != 0)  Password.toCharArray(DeviceSetting.wifiPassword, sizeof(DeviceSetting.wifiPassword));

    EEPROM.put (WifiSettingAddress, DeviceSetting);
    EEPROM.commit();
    Serial.println(DeviceSetting.wifiSSID);
    Serial.println(DeviceSetting.wifiPassword);

    if (IsNeedRestart) ESP.restart();
  } else server.send (503, "text/plane", "Fail");

  server.send (200, "text/plane", "OK");
}

void WIFIManager::OTAUpload() {
    HTTPUpload& upload = server.upload();
    
    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Начало OTA обновления: %s\n", upload.filename.c_str());
        #if defined(ESP8266)
            size_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace)) {
        #else
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        #endif
            Update.printError(Serial);
            server.send(500, "text/plain", "OTA начать не удалось");
            return;
        }
        
        Serial.println("OTA инициализировано");
    } 
    else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
            server.send(500, "text/plain", "Ошибка записи OTA");
            return;
        }
        Serial.printf("Записано: %d байт\n", upload.totalSize);
    }
    else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("OTA успешно завершено. Размер: %d байт\n", upload.totalSize);
            server.send(200, "text/plain", "Обновление успешно");
            delay(1000);
            ESP.restart();
        } else {
            Update.printError(Serial);
            server.send(500, "text/plain", "Ошибка завершения OTA");
        }
    }
}

void WIFIManager::StartServer()
{
    server.on("/styles.css", [this]() { GetCSS(); });
    server.on("/", [this]() { StartPage(); });
    server.on("/RegDeviceData", [this]() { RegDeviceData(); });  // Обработчик входящих данных
    server.on("/ScanWifi", [this]() { ScanWifi();} );            // Сканирование WIFI

    server.on("/update", HTTP_POST, 
        [this]() { server.send(200, "text/plain", "OK"); },
        [this]() { this->OTAUpload(); }
    );

    server.on("/GetWifiData", [this]() { GetWifiData();} );      // Запрос на получение данных о сохраненной сети

    server.on("/restart", [this]() { EspRestart();} );

    server.begin();
}