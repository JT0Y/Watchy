#ifndef WATCHY_BASE_H
#define WATCHY_BASE_H

#include <Watchy.h>
#include "fonts.h"
#include "icons.h"


// Color settings
#define DARKMODE            false
#define FOREGROUND_COLOR    (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)
#define BACKGROUND_COLOR    (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
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
        uint8_t getBattery();
        void vibTime();

        esp_sleep_wakeup_cause_t wakeup_reason;
    private:
        void _rtcConfig();    
        void _bmaConfig();
        static uint16_t _readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
        static uint16_t _writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
};

#endif
