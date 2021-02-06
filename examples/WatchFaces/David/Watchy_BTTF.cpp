#include "Watchy_BTTF.h"


// For more fonts look here:
// https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
//#define FONT_LARGE       Bohemian_Typewriter22pt7b
//#define FONT_MEDUM       Bohemian_Typewriter18pt7b
#define FONT_7_SEG_LARGE    Seven_Seg24pt7b
#define FONT_7_SEG_MEDIUM   Seven_Seg18pt7b

WatchyBTTF::WatchyBTTF(){

}


void WatchyBTTF::handleButtonPress(){
    WatchyBase::handleButtonPress();
    
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if(IS_DOUBLE_TAP){
        // NOP
    }
}


void WatchyBTTF::drawWatchFace(){
    WatchyBase::drawWatchFace();
    if(show_mqqt_data){
        return;
    }

    display.drawBitmap(0, 0, bttf, 200, 200, FOREGROUND_COLOR);
    drawTime();
    drawDate();
    drawSteps();
    drawBattery();
    //drawHelperGrid();
}


void WatchyBTTF::drawTime(){    
    display.setFont(&FONT_7_SEG_LARGE);
    display.setTextColor(BACKGROUND_COLOR);
    display.setCursor(95, 60);
    if(currentTime.Hour < 10){
        display.print("0");
    }
    display.println(currentTime.Hour);

    display.setCursor(145, 60);
    if(currentTime.Minute < 10){
        display.print("0");
    }
    display.println(currentTime.Minute);
}


void WatchyBTTF::drawDate(){
    display.setFont(&FONT_7_SEG_MEDIUM);
    display.setTextColor(BACKGROUND_COLOR);

    String month = monthShortStr(currentTime.Month);
    display.setCursor(15, 116);
    display.println(month);

    display.setCursor(78, 116);
    if(currentTime.Day < 10){
        display.print("0");    
    }
    display.println(currentTime.Day);    

    uint16_t year = currentTime.Year + YEAR_OFFSET;
    display.setCursor(127, 116);
    display.println(year); 
}


void WatchyBTTF::drawBattery(){   
    display.setFont(&FONT_7_SEG_MEDIUM);
    display.setTextColor(BACKGROUND_COLOR);

    int8_t percentage = getBattery();
    if(percentage < 10) {
        display.print("0");
    }

    display.setCursor(140, 181);
    display.println(percentage); 
}


void WatchyBTTF::drawSteps(){   
    display.setFont(&FONT_7_SEG_MEDIUM);
    display.setTextColor(BACKGROUND_COLOR);

    bool rtc_alarm = wakeup_reason == ESP_SLEEP_WAKEUP_EXT0;
    
    // Whenever we have a new hour, we can restart our step counting.
    // But only if its an rtc alarm - ignore button press etc.
    if(rtc_alarm && currentTime.Minute == 0 && currentTime.Hour == 0){
        sensor.resetStepCounter();
    }
    
    uint32_t steps = sensor.getCounter();
    display.setCursor(15, 181);
    display.println(steps); 
}