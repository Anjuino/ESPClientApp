#include "WIFIManager/WIFIManagerSmartHome.h"
class WIFIManagerSmartHome WIFIManagerSmartHome;

#if defined ESP32 && defined LED_CONTROLLER
  SET_LOOP_TASK_STACK_SIZE(24 * 1024);
  #include "Devices/DeviceLedController.h"
  class DeviceLedController Device(SmartDeviceSettingAddress, WIFIManagerSmartHome.GetServer());
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
