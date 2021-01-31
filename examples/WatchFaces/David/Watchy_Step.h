#ifndef WATCHY_STEP_H
#define WATCHY_STEP_H

#include "Watchy_Base.h"
#include "icons.h"
#include "fonts.h"

class WatchyStep : public WatchyBase {
    
    public:
        WatchyStep();
        void drawWatchFace();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawBattery();
        void handleButtonPress();

    private:
        esp_sleep_wakeup_cause_t wakeup_reason;
        
        float getMaxSteps();
        void startNewDay();
        int32_t getStepsOfDay();

        void _rtcConfig();    
        void _bmaConfig();
        static uint16_t _readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
        static uint16_t _writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
};

#endif
