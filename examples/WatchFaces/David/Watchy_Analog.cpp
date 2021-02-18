#include "Watchy_Analog.h"


// For more fonts look here:
// https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
//#define FONT_LARGE       Bohemian_Typewriter22pt7b
//#define FONT_MEDUM       Bohemian_Typewriter18pt7b
#define FONT    FreeSans12pt7b

WatchyAnalog::WatchyAnalog(){
    
}


void WatchyAnalog::handleButtonPress(){
    WatchyBase::handleButtonPress();
    
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if(IS_DOUBLE_TAP){
        dark_mode = dark_mode ? false : true;
        RTC.read(currentTime);
        showWatchFace(false);
        return;
    }
}


void WatchyAnalog::drawWatchFace(){
    WatchyBase::drawWatchFace();
    if(watchFaceDisabled()){
        return;
    }

    display.drawBitmap(0, 0, analog, 200, 200, FOREGROUND_COLOR);
    drawTime();
    drawDate();
    drawSteps();
    drawBattery();
    
    //drawHelperGrid();
}


void WatchyAnalog::printCentered(uint16_t x, uint16_t y, String text){
    int16_t  x1, y1;
    uint16_t w, h;

    display.getTextBounds(text, 40, 100, &x1, &y1, &w, &h);
    display.setCursor(x-w/2, y+h/2);
    display.println(text);
}


void WatchyAnalog::drawTime(){    
    int currentMinute = currentTime.Minute;
    int minuteAngle = currentMinute * 6;
    double radMinute = ((minuteAngle + 180) * 71) / 4068.0;
    double mx1 = 100 - (sin(radMinute) * 85);
    double my1 = 100 + (cos(radMinute) * 85);
    for(int i=-2; i<3; i++){
        display.drawLine(100+i, 100, (int)mx1+i, (int)my1, FOREGROUND_COLOR);
        display.drawLine(100, 100+i, (int)mx1, (int)my1+i, FOREGROUND_COLOR);
    }

    //hour pointer
    int currentHour= currentTime.Hour;
    double hourAngle = (currentHour * 30) + currentMinute * 0.5;
    double radHour = ((hourAngle + 180) * 71) / 4068.0;
    double hx1 = 100 - (sin(radHour) * 45);
    double hy1 = 100 + (cos(radHour) * 45);

    for(int i=-2; i<3; i++){
        display.drawLine(100+i, 100, (int)hx1+i, (int)hy1, FOREGROUND_COLOR);
        display.drawLine(100, 100+i, (int)hx1, (int)hy1+i, FOREGROUND_COLOR);
    }
}


void WatchyAnalog::drawDate(){
    display.setFont(&FONT);
    display.setTextColor(BACKGROUND_COLOR);

    String dayStr = String(currentTime.Day);
    dayStr = currentTime.Day < 10 ? "0" + dayStr : dayStr;
    printCentered(134, 100, dayStr);
}


void WatchyAnalog::drawBattery(){   
    display.setFont(&FONT);
    display.setTextColor(FOREGROUND_COLOR);

    int8_t bat = getBattery();
    String batStr = String(bat);
    batStr = bat < 10 ? "0" + batStr : batStr;
    batStr = batStr + "%";
    printCentered(100, 55, batStr);
}


void WatchyAnalog::drawSteps(){   
    display.setFont(&FONT);
    display.setTextColor(FOREGROUND_COLOR);

    bool rtc_alarm = wakeup_reason == ESP_SLEEP_WAKEUP_EXT0;
    
    // Whenever we have a new hour, we can restart our step counting.
    // But only if its an rtc alarm - ignore button press etc.
    if(rtc_alarm && currentTime.Minute == 0 && currentTime.Hour == 0){
        sensor.resetStepCounter();
    }
    
    uint32_t steps = sensor.getCounter();
    printCentered(100, 145, String(steps));
}