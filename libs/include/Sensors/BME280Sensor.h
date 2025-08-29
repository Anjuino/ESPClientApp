#pragma once
#ifdef ESP8266

#include "SensorBase.h"


class BME280Sensor: public SensorT, public SensorH {

   public:

      void Init() {return;};
      
      float GetTemperature() override {return 0;};
      float GetHumidity()    override {return 0;};
};

#endif


