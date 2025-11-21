#include "SensorBase.h"
#include <DHT.h>

#define DHTTYPE DHT22

#pragma once
#ifdef ESP8266

class DHTSensor: public SensorT, public SensorH {

    private:
        DHT dht{D6, DHTTYPE};
    
    public:

        void Init()
        {
            dht.begin();
        };
        
        float GetTemperature() override 
        {
            return dht.readTemperature();
        };

        float GetHumidity() override 
        {
            return dht.readHumidity();
        };
};

#endif