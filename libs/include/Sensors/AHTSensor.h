#pragma once
#ifdef ESP8266

#include "SensorBase.h"
#include <Adafruit_AHTX0.h>

class AHTSensor: public SensorT, public SensorH {

    private:
        Adafruit_AHTX0 aht;
    
    public:

        void Init()
        {
            aht.begin();
        };
        
        float GetTemperature() override 
        {
            sensors_event_t humidity, temp;
            aht.getEvent(&humidity, &temp);
            return temp.temperature;
        };

        float GetHumidity() override 
        {
            sensors_event_t humidity, temp;
            aht.getEvent(&humidity, &temp);
            return humidity.relative_humidity;
        };
};

#endif