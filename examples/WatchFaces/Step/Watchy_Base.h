#ifndef WATCHY_BASE_H
#define WATCHY_BASE_H

#include <Watchy.h>
#include "fonts.h"
#include "icons.h"


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
