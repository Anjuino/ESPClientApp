#include "WIFIManager/WIFIManagerSmartHome.h"
class WIFIManagerSmartHome WIFIManagerSmartHome;

#if defined ESP8266 && defined TELEMETRY_CONTROLLER
  #include "Devices/DeviceTelemetry.h"
  class DeviceTelemetry Device(SmartDeviceSettingAddress, WIFIManagerSmartHome.GetServer());
#endif

void setup() 
{
  EEPROM.begin(512);
  Serial.begin(9600);

  WIFIManagerSmartHome.Start(false);

  Device.Init();
}

void loop() 
{
  Device.Loop();
}
