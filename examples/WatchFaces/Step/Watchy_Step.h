#ifndef WATCHY_7_SEG_H
#define WATCHY_7_SEG_H

#include <Watchy.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include "icons.h"


class WatchyStep : public Watchy {
    private:
        float getMaxSteps();
    public:
        WatchyStep();
        void drawWatchFace();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawBattery();
};

#endif
