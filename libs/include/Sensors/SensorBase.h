#pragma once
#ifdef ESP8266

#include <vector>
#include <memory>
#include <variant>

// Базовый интерфейс для всех датчиков
class SensorBase {
    public:
        virtual ~SensorBase() = default;
        virtual void Init() = 0;
        virtual std::variant<float, uint16_t> GetData() = 0;
};

// Классы под разные типы датчиков
class SensorT : public SensorBase {
    public:
        virtual float GetTemperature() = 0;
        void Init() override = 0;
        std::variant<float, uint16_t> GetData() override { return GetTemperature(); }
};

class SensorH : public SensorBase {
    public:
        virtual float GetHumidity() = 0;
        void Init() override = 0;
        std::variant<float, uint16_t> GetData() override { return GetHumidity(); }
};

class SensorCO2 : public SensorBase {
    public:
        virtual uint16_t GetCO2() = 0;
        void Init() override = 0;
        std::variant<float, uint16_t> GetData() override { return GetCO2(); }
};

#endif