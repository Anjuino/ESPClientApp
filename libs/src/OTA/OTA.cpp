#include "OTA/OTA.h"

void StartOTA() {
  Serial.println("Начинаем OTA обновление...");
  
  String URL_OTA = String("http://") + ADDR + String(":") + String(PORT) + "/DeviceSetting/GetFirmware";

  Serial.println(URL_OTA);

  WiFiClient client;
  HTTPClient http;
    
  #ifdef ESP32
    http.begin(URL_OTA);  
  #else
    http.begin(client, URL_OTA);  
  #endif
    
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    int len = http.getSize();

    if (Update.begin(len)) {
      Serial.printf("Размер прошивки: %d байт\n", len);

      WiFiClient *stream = http.getStreamPtr();
      size_t written = Update.writeStream(*stream);

      if (written == len) Serial.printf("Прошивка загружена: %d байт\n", written);
      else Serial.printf("Загружено только %d из %d байт\n", written, len);

      if (Update.end()) {
        Serial.println("OTA завершена!");
        if (Update.isFinished()) {
          Serial.println("Обновление успешно. Перезагрузка...");
          ESP.restart();
        } else Serial.println("OTA не завершена!");
      } else Serial.println("Ошибка OTA: " + String(Update.getError()));
    } else Serial.println("Недостаточно места для OTA");
  } else Serial.println("Ошибка загрузки прошивки: " + String(httpCode));

  http.end();
}