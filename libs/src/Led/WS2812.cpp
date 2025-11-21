#ifdef ESP32
#include "Led/WS2812.h"

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

void WS2812::Init(uint16_t SettingsAddress, uint8_t Pin, bool IsSensorDetected)
{
   this->SettingsAddress = SettingsAddress;
   this->IsSensorDetected = IsSensorDetected;

   if (this->SettingsAddress) {
      Led.setPin(Pin);
      Led.updateType(NEO_GRB + NEO_KHZ800);
      Led.setBrightness(State.Blind);
      
      EEPROM.get(this->SettingsAddress, Setting);
      if (Setting.LedCount > 1000) Setting.LedCount = 50;
      
      Led.updateLength(Setting.LedCount);
   }
}

void WS2812::ChangeRGB()
{
   r1 = random (256);
   g1 = random (256);
   b1 = random (256);
   r2 = random (256);
   g2 = random (256);
   b2 = random (256);
}

void WS2812::SetColor(uint8_t r, uint8_t g, uint8_t b) 
{
   for (uint16_t i = 0; i < Led.numPixels (); i++) {
      Led.setPixelColor (i, r, g, b);
   }
   Led.show ();
}

void WS2812::SetSpeed(uint8_t SpeedRecv) 
{
  State.Speed = SpeedRecv;
}

uint32_t WS2812::Wheel(byte WheelPos) 
{
   if (WheelPos < 85) return Led.Color (WheelPos * 3, 255 - WheelPos * 3, 0);
   else if( WheelPos < 170) {
      WheelPos -= 85;
      return Led.Color (255 - WheelPos * 3, 0, WheelPos * 3);
   } 
   else {
      WheelPos -= 170;
      return Led.Color (0, WheelPos * 3, 255 - WheelPos * 3);
   }
}

void WS2812::RainbowCycle(uint8_t wait) {
   static uint16_t z = 0;
   for(uint16_t i=0; i< Led.numPixels (); i++) {
      Led.setPixelColor(i, Wheel (((i * 256 / Led.numPixels ()) + z) & 255));
   }

   vTaskDelay(pdMS_TO_TICKS(20 - wait));
   Led.show ();
   z++;

   if (z == 256) z = 0;
}

void WS2812::RandomLight(uint8_t wait)
{
   uint16_t RandNumPix = random (Led.numPixels ());

   Led.setPixelColor (RandNumPix, Led.Color (random (256), random (256), random (256)));
   Led.show (); 

   vTaskDelay(pdMS_TO_TICKS(120 - (wait*4))); 

   Led.setPixelColor (RandNumPix, Led.Color (0, 0, 0)); 
   Led.show (); 
}

void WS2812::RGBLoop(uint8_t wait)
{  
   static int Mode = 0;
   static uint8_t Blind = 0;
   static bool IsUp = true;

   if (IsUp) {
      switch (Mode) {
         case 0: SetColor (Blind,0,0); break;
         case 1: SetColor (0,Blind,0); break;
         case 2: SetColor (0,0,Blind); break;
         case 3: SetColor (0,Blind,Blind); break;
         case 4: SetColor (Blind,0,Blind); break;
         case 5: SetColor (Blind,Blind,0); break;
      }

      vTaskDelay(pdMS_TO_TICKS(23 - wait)); 
      Blind++;

      if (Blind == 255) IsUp = false;
   }

   if (!IsUp) {
      switch(Mode) {
         case 0: SetColor (Blind,0,0); break;
         case 1: SetColor (0,Blind,0); break;
         case 2: SetColor (0,0,Blind); break;
         case 3: SetColor (0,Blind,Blind); break;
         case 4: SetColor (Blind,0,Blind); break;
         case 5: SetColor (Blind,Blind,0); break;
      }

      vTaskDelay(pdMS_TO_TICKS(23 - wait)); 
      Blind--;

      if (Blind == 0) {
         IsUp = true;
         Mode++;
         if (Mode == 6) Mode = 0;
      }
   }
}

void WS2812::Sparkle(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait) 
{ 
  SetColor(red, green, blue);
  int Pixel = random(Led.numPixels ());
  Led.setPixelColor(Pixel,255,255,255);
  Led.show();
  vTaskDelay(pdMS_TO_TICKS(20)); 
  Led.setPixelColor(Pixel,red,green,blue);
  Led.show();
  vTaskDelay(pdMS_TO_TICKS(1000 - (45 * wait))); 
}

void WS2812::RunColor (uint8_t wait) 
{
   static int posUp = 0; 
   static int posDown = 0; 
   static int count; 

   Led.setPixelColor (posUp, Led.Color (r1, g1, b1));
   posUp++;

   vTaskDelay(pdMS_TO_TICKS(81 - (4 * wait)));  

   Led.setPixelColor (posDown, Led.Color (r2, g2, b2)); 
   posDown--;

   Led.show (); 

   if (posUp == Led.numPixels ()) posUp = 0;

   if (posDown == 0) posDown = Led.numPixels ();

   count++;

   if (count == Led.numPixels () / 2) {
      ChangeRGB ();
   }

   if (count == Led.numPixels ()) {
      ChangeRGB ();
      count = 0;
   }
}

void WS2812::RunningLights (uint8_t red, uint8_t green, uint8_t blue, uint32_t wait) {
   static int Position = 0;
   static int j = 0;
   Position++;

   for (int i = 0; i < Led.numPixels (); i++) {
      Led.setPixelColor (i,((sin (i+Position) * 127 + 128)/255)*red,
                        ((sin (i+Position) * 127 + 128)/255)*green,
                        ((sin (i+Position) * 127 + 128)/255)*blue);
   }

   Led.show ();
   vTaskDelay(pdMS_TO_TICKS(70 - wait));  

   j++;
   if (j == (Led.numPixels ()*2)) {
      Position = 0;
      j = 0;
   }
}

void WS2812::RunStr (uint32_t wait)
{ 
   static int pos = 1;
   static bool flag = true;

   Led.setPixelColor (pos - 2, Led.Color (State.r, State.g, State.b)); 
   Led.setPixelColor (pos - 1, Led.Color (State.r, State.g, State.b)); 
   Led.setPixelColor (pos, Led.Color (State.r, State.g, State.b)); 
   Led.setPixelColor (pos + 1, Led.Color (State.r, State.g, State.b)); 
   Led.setPixelColor (pos + 2, Led.Color (State.r, State.g, State.b)); 
   Led.show (); 

   vTaskDelay(pdMS_TO_TICKS(81 - (4 * wait)));  

   Led.setPixelColor (pos - 2, Led.Color (0, 0, 0)); 
   Led.setPixelColor (pos - 1, Led.Color (0, 0, 0)); 
   Led.setPixelColor (pos, Led.Color (0, 0, 0)); 
   Led.setPixelColor (pos + 1, Led.Color (0, 0, 0)); 
   Led.setPixelColor (pos + 2, Led.Color (0, 0, 0)); 
   Led.show (); 

   if (flag) pos = (pos + 1);
   else      pos = (pos - 1);

   if (pos == Led.numPixels ()) flag = false;

   if (pos == 0) flag = true;
}

void WS2812::RunStr2 (uint32_t wait)
{ 
   static int pos = 1;                     
   static int pos2 = Led.numPixels ();   
   static bool flag = true;

   Led.setPixelColor (pos - 1, Led.Color (r1, g1, b1)); 
   Led.setPixelColor (pos, Led.Color (r1, g1, b1)); 
   Led.setPixelColor (pos + 1, Led.Color (r1, g1, b1)); 

   Led.setPixelColor (pos2 - 1, Led.Color (r2, g2, b2)); 
   Led.setPixelColor (pos2, Led.Color (r2, g2, b2)); 
   Led.setPixelColor (pos2 + 1, Led.Color (r2, g2, b2)); 

   Led.show (); 

   vTaskDelay(pdMS_TO_TICKS(81 - (4 * wait)));  

   Led.setPixelColor (pos - 1, Led.Color (0, 0, 0)); 
   Led.setPixelColor (pos, Led.Color (0, 0, 0)); 
   Led.setPixelColor (pos + 1, Led.Color (0, 0, 0));

   Led.setPixelColor (pos2 - 1, Led.Color (0, 0, 0)); 
   Led.setPixelColor (pos2, Led.Color (0, 0, 0)); 
   Led.setPixelColor (pos2 + 1, Led.Color (0, 0, 0));

   Led.show (); 

   if (flag) {
      pos = pos + 1; 
      pos2 = pos2 - 1 ;
   }
   else {
      pos = pos - 1;
      pos2 = pos2 + 1;
   }

   if (pos == Led.numPixels() / 2) ChangeRGB();

   if (pos == Led.numPixels()) {
      ChangeRGB();
      flag = false;
   }

   if (pos == 0) flag = true;
}

void WS2812::Chaos(uint32_t wait)
{
   static uint8_t randcolor = 0;
   for (uint16_t i = 0; i < Led.numPixels (); i++) {
   randcolor = random (10);
      if (randcolor == 0) Led.setPixelColor (i, random (256), 0, 0);
      if (randcolor == 1) Led.setPixelColor (i, 0, random (256), 0);
      if (randcolor == 2) Led.setPixelColor (i, 0, 0, random (256));
      if (randcolor == 3) Led.setPixelColor (i, random (256), random (256), 0);
      if (randcolor == 4) Led.setPixelColor (i, random (256), 0, random (256));
      if (randcolor == 5) Led.setPixelColor (i, 0, random (256), random (256));
      if (randcolor == 6) Led.setPixelColor (i, random (256), random (256), random (256));
      if (randcolor == 7) Led.setPixelColor (i, random (128), random (256), random (128));
      if (randcolor == 8) Led.setPixelColor (i, random (128), random (256), random (256));
      if (randcolor == 9) Led.setPixelColor (i, random (256), random (128), random (128));
   }
   Led.show ();
   vTaskDelay(pdMS_TO_TICKS(300 - (10 * wait)));  
}

void WS2812::SetState(LedMode Mode) 
{
   State.Mode = Mode.Mode;
   State.r = Mode.r;
   State.g = Mode.g;
   State.b = Mode.b;

   r1 = Mode.r;
   g1 = Mode.g;
   b1 = Mode.b;

   if (State.Mode == STATIC) FlagOneOn = true;
}

void WS2812::SetBlind(uint8_t Blind)
{
   State.Blind = Blind;
   Led.setBrightness (State.Blind * 2);
   Led.show ();
}

void WS2812::UpdateLedCount(uint16_t LedCount)
{
   if (LedCount > 0 && LedCount < 600) Led.updateLength(LedCount);
}

void WS2812::UpdateSetting(LedSetting iSetting)
{
   if (Setting.LedCount != iSetting.LedCount) UpdateLedCount(iSetting.LedCount);
   
   Setting = iSetting;

   EEPROM.put(SettingsAddress, Setting);
   EEPROM.commit();
}

void WS2812::Loop() 
{
   switch (State.Mode) {

      case OFF:
      {
         SetColor(0, 0, 0);
         State.Mode = 0;
         break;
      }
      case NOTHING:
      {
         break;
      }
      
      case RAINBOW:
      {
         RainbowCycle(State.Speed);
         break; 
      }

      case RUNSTR:
      {
         RunStr(State.Speed);
         break;
      }

      case STROBE:
      {
         RunColor(State.Speed);
         break;
      }

      case RANDOMLIGHT:
      {
         RandomLight(State.Speed);
         break;
      }

      case SPARK:
      {
         Sparkle(16, 16, 16, State.Speed);
         break;
      }

      case FADE:
      {
         RGBLoop(State.Speed);
         break;
      } 

      case RUNLIGHT:
      {
         RunningLights(State.r, State.g ,State.b, State.Speed);
         break;
      }
      
      case CHAOS:
      {
         Chaos(State.Speed);
         break;
      }

      case RUNSTR2:
      {
         RunStr2(State.Speed);
         break;
      }

      case STATIC:
      {  
         if (FlagOneOn) {
            SetColor(State.r, State.g, State.b);
            FlagOneOn = false;
         }
         break;
      }
   }
}

#endif