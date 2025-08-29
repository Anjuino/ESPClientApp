#pragma once
#ifdef ESP8266

#include "SensorBase.h"
#include "Arduino.h"

class TestSensor: public SensorT, public SensorH, public SensorCO2 {

    private:
    
    public:

        void Init()
        {

        };
        
        float GetTemperature(/*float *result*/) override 
        {
            return 7.77 + random(0, 80);
        };

        float GetHumidity(/*float *result*/) override 
        {

            return 10.54 + random(0, 80);
        };

        uint16_t GetCO2(/*float *result*/) override 
        {

            return random(400, 700);
        };
};

#endif