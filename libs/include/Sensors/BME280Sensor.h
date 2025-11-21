#pragma once
#ifdef ESP8266

#include "SensorBase.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

class BME280Sensor: public SensorT, public SensorH {

   private:
      Adafruit_BME280 bme;

   public:

      void Init() { bme.begin(0x76); };
      
      float GetTemperature() override { return bme.readTemperature() - 2.5; };
      float GetHumidity()    override { return bme.readHumidity(); };
};

#endif


