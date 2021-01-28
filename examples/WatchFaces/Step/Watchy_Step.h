#ifndef WATCHY_7_SEG_H
#define WATCHY_7_SEG_H

#include <Watchy.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "icons.h"


class WatchyStep : public Watchy {
    public:
        WatchyStep();
        void drawWatchFace();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawBattery();

        void init();
        void handleButtonPress();


    private:
        float getMaxSteps();
        void startNewDay();

        void _rtcConfig();    
        void _bmaConfig();
        static uint16_t _readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
        static uint16_t _writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len);
};

#endif
