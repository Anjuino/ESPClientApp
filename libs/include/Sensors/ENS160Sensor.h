#pragma once
#ifdef ESP8266
#include "SensorBase.h"
#include "SparkFun_ENS160.h"

class ENS160Sensor: public SensorCO2 {

    private:
        SparkFun_ENS160 ens160;
        
    public:

        void Init() 
        {
            ens160.begin();
            ens160.setOperatingMode(SFE_ENS160_RESET);
            delay(100);
            ens160.setOperatingMode(SFE_ENS160_STANDARD);
        };
      
        uint16_t GetCO2() override 
        {
            return ens160.getECO2();
        };
};

#endif