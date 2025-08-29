#include "WIFIManager.h"
#define SmartDeviceSettingAddress 50       // Адрес настроек для eeprom

class WIFIManagerSmartHome:public WIFIManager {

    private:

        struct{
            bool SettingIsEmpty;
            char Token[20];
            char ServerIp[15];
        } DeviceSmartHomeSetting;

        void GetToken();
        void GetIpServer();
        
    public:

        WIFIManagerSmartHome();
        ~WIFIManagerSmartHome() {};

        void Init(bool IsNeedEEPROM) override;
        void Start(bool IsNeedEEPROM) override;

        void StartServer() override;

        void RegDeviceData(bool IsNeedRestart = true) override;
        void ResetSettings();
};