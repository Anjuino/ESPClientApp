#pragma once
#ifdef ESP32

#define NOTHING     0

#define RAINBOW     1
#define RUNSTR      2
#define STROBE      3
#define RANDOMLIGHT 4
#define SPARK       5
#define FADE        6
#define RUNLIGHT    7
#define CHAOS       8
#define RUNSTR2     9

#define STATIC     249
#define OFF        250

#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#include "EEPROM.h"

struct LedMode {
   uint8_t Mode;
   uint8_t r = 0; 
   uint8_t g = 0; 
   uint8_t b = 0; 
   uint8_t Speed = 10;
   uint8_t Blind = 50;
};

struct LedSetting {
   uint16_t LedCount;
   bool IsDetectedMove = false;
};

class WS2812 {

   private:

      LedMode State;
      LedSetting Setting;

      Preferences preferences;
      
      bool FlagOneOn = true;

      uint8_t r1 = 255;
      uint8_t g1 = 0; 
      uint8_t b1 = 0;

      uint8_t r2 = 0;
      uint8_t g2 = 0; 
      uint8_t b2 = 255;

      uint16_t SettingsAddress = 0;

      bool IsSensorDetected = false;

      uint32_t Wheel (uint8_t WheelPos);
      void SetColor(uint8_t r, uint8_t g, uint8_t b);
      void ChangeRGB();   
      void RunStr(uint32_t wait);
      void RunStr2(uint32_t wait);
      void Chaos(uint32_t wait);
      void RainbowCycle(uint8_t wait);
      void RunningLights (uint8_t red, uint8_t green, uint8_t blue, uint32_t wait);
      void RandomLight(uint8_t wait);
      void RGBLoop(uint8_t wait);
      void Sparkle(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait);
      void RunColor(uint8_t wait);

      Adafruit_NeoPixel Led; 

   public:
      WS2812()  {};
      ~WS2812() {};

      void Init(uint16_t SettingsAddress, uint8_t Pin, bool IsSensorDetected = false); 
      void Loop();

      void SetBlind(uint8_t Blind);
      void SetSpeed(uint8_t SpeedRecv);
      void SetState (LedMode Mode);
      void UpdateLedCount(uint16_t LedCount);
      void UpdateSetting(LedSetting iSetting);

      LedSetting GetSetting() {return Setting;};
      LedMode GetState() {return State;};

      void initPreferences() {
         preferences.begin("led", false);
      }
         
      void saveState() {
         preferences.putBytes("state", &State, sizeof(State));
      }
         
      void loadState() {
         size_t readSize = preferences.getBytes("state", &State, sizeof(State));
         if (readSize != sizeof(State)) {
            State.Mode = 0;
            State.r = 0;
            State.g = 0;
            State.b = 0;
            State.Speed = 10;
            State.Blind = 50;
            saveState();
         }
         
         r1 = State.r;
         g1 = State.g;
         b1 = State.b;
         if (State.Mode == STATIC) FlagOneOn = true;

         Led.setBrightness(State.Blind * 2);
         
      }
};

#endif