#ifndef WATCHY_BASE_H
#define WATCHY_BASE_H

#include <Watchy.h>
#include "fonts.h"
#include "icons.h"
#include <WiFiManager.h>
#include "Images/smart_home.h"
#include <PubSubClient.h>

// IOT WIFI SETTINGS - CONTROL VIA HTTP GET ON URL
#define WIFI_SSID      "ENTER_HERE"
#define WIFI_PASS      "ENTER_HERE"

#define MQTT_BROKER    "ENTER_HERE"
#define MQTT_TOPIC     "ENTER_HERE"
#define MQTT_PAYLOAD   "ENTER_HERE"

#if __has_include("config.h") && __has_include(<stdint.h>)
# include "config.h"
#endif


extern RTC_DATA_ATTR bool show_mqqt_data;

// Color settings
extern RTC_DATA_ATTR bool dark_mode;
#define FOREGROUND_COLOR    (dark_mode ? GxEPD_WHITE : GxEPD_BLACK)
#define BACKGROUND_COLOR    (dark_mode ? GxEPD_BLACK : GxEPD_WHITE)
#define GRAPH_COLOR         FOREGROUND_COLOR
#define BATTERY_OFFSET      0.25    // This offset is different for each watch [V]

// Other settings 
#define DOUBLE_TAP_TIME     3       // Time between two double taps [s]
#define EXT_INT_MASK        MENU_BTN_MASK|BACK_BTN_MASK|UP_BTN_MASK|DOWN_BTN_MASK|ACC_INT_MASK

class WatchyBase : public Watchy {
    public:
        WatchyBase();
        virtual void drawWatchFace();
        virtual void init();
        virtual void handleButtonPress();
        virtual void deepSleep();
        void vibrate(uint8_t times=1, uint32_t delay_time=200);
        uint8_t getBattery();
        bool connectWiFi();
        void disconnectWiFi();
        uint8_t openDoor();
        uint8_t loadMqqtData();
        void drawHelperGrid();
        void drawMqqtData();

        esp_sleep_wakeup_cause_t wakeup_reason;
    private:
        void _rtcConfig();    
        void _bmaConfig();
        static uint16_t _readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
        static uint16_t _writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
};

#endif
