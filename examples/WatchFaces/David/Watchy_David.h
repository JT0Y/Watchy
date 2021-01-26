#ifndef WATCHY_7_SEG_H
#define WATCHY_7_SEG_H

#include <Watchy.h>
#include "Seven_Segment10pt7b.h"
#include "DSEG7_Classic_Regular_15.h"
#include "DSEG7_Classic_Bold_25.h"
#include "DSEG7_Classic_Regular_39.h"
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include "icons.h"

class WatchyDavid : public Watchy{
    public:
        WatchyDavid();
        void drawWatchFace();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawBattery();
};

#endif
