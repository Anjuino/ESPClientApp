#ifdef ESP32
    #include <HTTPClient.h>
    #include <Update.h>
#else
    #include <ESP8266HTTPClient.h>
    #include <ESP8266httpUpdate.h>
#endif

extern const char* ADDR;
extern uint16_t    PORT;

void StartOTA();