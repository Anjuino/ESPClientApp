#pragma once
#ifdef ESP8266

#include "SensorBase.h"
#include <microDS18B20.h>

class DS18B20 : public SensorT {
   private:
      MicroDS18B20<0> sensor;

   public:

      void Init() {return;};
      float GetTemperature() override 
      {
         sensor.requestTemp();
         delay(50);
         return sensor.getTemp();
      };

};

#endif