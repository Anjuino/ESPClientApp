#if defined ESP32 && defined LED_CONTROLLER
#include "Devices/DeviceLedController.h"

TaskHandle_t Led_task;
void DeviceLedController::LedCode(void *pvParameters) {
   DeviceLedController* controller = (DeviceLedController*)pvParameters;
   controller->Led.Init(AddressSettings, 27, SensorDetectedMove);
   
   while (true) {
      controller->Led.Loop();
      vTaskDelay(10);
   }
}

void DeviceLedController::ParseIncomingMessage(JsonDocument doc, String TypeMesseage)
{
   Device::ParseIncomingMessage(doc, TypeMesseage);

   if (TypeMesseage == "GetState") SendState();

   if (TypeMesseage == "SetStateToLed")      SetState(doc);
   if (TypeMesseage == "SetSpeedToLed")      SetSpeed(doc);
   if (TypeMesseage == "SetBrightnessToLed") SetBrightness(doc);

   if (TypeMesseage == "UpdateSettingLed") UpdateSettingLed(doc);
   if (TypeMesseage == "GetSettingLed") SendLedSetting();
}

void DeviceLedController::AppStart()
{
   JsonDocument doc;

   // Заполняем объект данными
   doc["TypeMesseage"] = "Authentication";
   doc["Token"]        = Settings.Token;
   doc["TypeDevice"]   = "LedController";
   doc["Build"]        = build;

   SendMesseageToServer(doc);
}

void DeviceLedController::Init()
{
   Device::Init();
   Led.Init(AddressSettings, 27, SensorDetectedMove);
   //xTaskCreatePinnedToCore(LedCode, "LedCode", 1024 * 20, this, 10, &Led_task, 1);
}

void DeviceLedController::SetState(JsonDocument doc)
{
   LedMode State;
   uint8_t TypeError = 0;

   if (doc.containsKey("Mode") && doc["Mode"].is<int>()) State.Mode = doc["Mode"];
   else                                                  TypeError = 1;

   if (doc.containsKey("ColorR") && doc["ColorR"].is<int>()) State.r = doc["ColorR"];
   else                                                      TypeError = 2;

   if (doc.containsKey("ColorG") && doc["ColorG"].is<int>()) State.g = doc["ColorG"];
   else                                                      TypeError = 3;

   if (doc.containsKey("ColorB") && doc["ColorB"].is<int>()) State.b = doc["ColorB"];
   else                                                      TypeError = 4;

   if (TypeError) {
      String TypeParam;
      if (TypeError == 1) {
         TypeParam = "Mode";
         TypeError = 0;
      }
      if (TypeError == 2) {
         TypeParam = "ColorR";
         TypeError = 0;
      }
      if (TypeError == 3) {
         TypeParam = "ColorG";
         TypeError = 0;
      }
      if (TypeError == 4) {
         TypeParam = "ColorB";
         TypeError = 0;
      }

      String Error = "Параметра " + TypeParam + " нет или он не является числом";
      SendLog(Error);
   } 
   else Led.SetState(State);
}

void DeviceLedController::SetSpeed(JsonDocument doc)
{
   uint8_t TypeError = 0;
   uint8_t Speed = 0;
   if (doc.containsKey("Speed") && doc["Speed"].is<int>()) Speed = doc["Speed"];
   else                                                    TypeError = 1;

   if (TypeError) {
      String TypeParam;
      if (TypeError == 1) TypeParam = "Speed";

      String Error = "Параметра " + TypeParam + " нет или он не является числом";
      SendLog(Error);
   } 
   else Led.SetSpeed(Speed);
}

void DeviceLedController::SetBrightness(JsonDocument doc)
{
   uint8_t TypeError = 0;
   uint8_t Blind = 0;
   if (doc.containsKey("Brightness") && doc["Brightness"].is<int>()) Blind = doc["Brightness"];
   else                                                              TypeError = 1;

   if (TypeError) {
      String TypeParam;
      if (TypeError == 1) TypeParam = "Brightness";

      String Error = "Параметра " + TypeParam + " нет или он не является числом";
      SendLog(Error);
   } 
   else Led.SetBlind(Blind);
}

void DeviceLedController::UpdateSettingLed(JsonDocument doc)
{
   uint8_t TypeError = 0;
   LedSetting Setting;

   if (doc.containsKey("LedCount") && doc["LedCount"].is<int>()) Setting.LedCount = doc["LedCount"];
   else                                                          TypeError = 1;

   if (SensorDetectedMove) {
      if (doc.containsKey("IsDetectedMove") && doc["IsDetectedMove"].is<bool>()) Setting.IsDetectedMove = doc["IsDetectedMove"];
      else                                                                       TypeError = 2;
   }

   if (TypeError) {
      String TypeParam;
      if (TypeError == 1) TypeParam = "LedCount";
      if (TypeError == 2) TypeParam = "IsDetectedMove";

      String Error = "Параметра " + TypeParam + " нет или он не является числом";
      SendLog(Error);
   } 
   else Led.UpdateSetting(Setting);
}

void DeviceLedController::SendLedSetting()
{
   LedSetting Setting = Led.GetSetting();
   JsonDocument doc;

   doc["TypeMesseage"] = "LedSetting";

   doc["CountLed"] = Setting.LedCount;

   if (SensorDetectedMove) {
      doc["IsSensorDetectedMove"] = true;
      doc["IsDetectedMove"] = Setting.IsDetectedMove;
   }
   else doc["IsSensorDetectedMove"] = false;

   SendMesseageToServer(doc);
}

void DeviceLedController::SendState()
{
   LedMode State = Led.GetState();
   JsonDocument doc;

   doc["TypeMesseage"] = "State";
   doc["Mode"] = State.Mode;

   doc["ColorR"] = State.r;
   doc["ColorG"] = State.g;
   doc["ColorB"] = State.b;

   doc["Speed"] = State.Speed;
   doc["Brightness"] = State.Blind;
   
   SendMesseageToServer(doc);
}

void DeviceLedController::Loop()
{
   Device::Loop();

   Led.Loop();
}

#endif
