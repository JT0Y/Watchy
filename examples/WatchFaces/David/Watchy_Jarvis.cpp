#include "Watchy_Jarvis.h"


// For more fonts look here:
// https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
#define FONT_LARGE_BOLD  FreeSansBold24pt7b
#define FONT_LARGE       FreeSans24pt7b
#define FONT_MEDUM_BOLD  FreeSansBold12pt7b
#define FONT_MEDUM       FreeSans12pt7b
#define FONT_SMALL_BOLD  FreeSansBold9pt7b
#define FONT_SMALL       FreeSans9pt7b


WatchyJarvis::WatchyJarvis(){

}


void WatchyJarvis::handleButtonPress(){
    WatchyBase::handleButtonPress();
    
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if(wakeupBit & UP_BTN_MASK && guiState == WATCHFACE_STATE){
        // NOP
    }
}


void WatchyJarvis::drawWatchFace(){
    WatchyBase::drawWatchFace();
    drawTime();
    drawSteps();
    drawBattery();
    //drawDate();
}


void WatchyJarvis::drawTime(){
    display.setFont(&FONT_MEDUM);
    display.setCursor(120, 42);
    if(currentTime.Hour < 10){
        display.print("0");
    }
    display.print(currentTime.Hour);
    display.print(":");
    if(currentTime.Minute < 10){
        display.print("0");
    }  
    display.println(currentTime.Minute);  
}


void WatchyJarvis::drawDate(){
    int16_t  x1, y1;
    uint16_t w, h;

    display.setFont(&FONT_MEDUM);
    String dayOfWeek = dayShortStr(currentTime.Wday);
    display.getTextBounds(dayOfWeek, 55, 195, &x1, &y1, &w, &h);
    display.setCursor(200-20-w, 25);
    display.println(dayOfWeek);

    display.getTextBounds("00", 55, 195, &x1, &y1, &w, &h);
    display.setCursor(200-20-w, 45);
    if(currentTime.Day < 10){
        display.print("0");    
    }
    display.println(currentTime.Day);
}


void WatchyJarvis::drawBattery(){   
    drawCircle(20, 170, 30, FOREGROUND_COLOR, 3);
    display.fillRect(30, 170, 40, 80, BACKGROUND_COLOR);

    drawCircle(20, 170, 45, FOREGROUND_COLOR, 5);

    int8_t percentage = getBattery();
    percentage = min((int8_t) 99, percentage);    
    display.setFont(&FONT_MEDUM);
    display.setCursor(5, 175);
    if(percentage < 10) {
        display.print("0");
    }
    display.println(percentage);

    display.setFont(&FONT_SMALL);
    display.setCursor(60, 140);
    display.println("POWER");

}


void WatchyJarvis::drawSteps(){   
    bool rtc_alarm = wakeup_reason == ESP_SLEEP_WAKEUP_EXT0;
    
    // Whenever we have a new hour, we can restart our step counting.
    // But only if its an rtc alarm - ignore button press etc.
    if(rtc_alarm && currentTime.Minute == 0){
        sensor.resetStepCounter();
    }
    
    display.setFont(&FONT_MEDUM_BOLD);    

    uint32_t steps = sensor.getCounter();
    display.setCursor(165, 175);
    if(steps >= 1000){
        steps /= 1000;
        display.print(steps);
        display.println("k");
    } else {
        display.println(steps);
    }

    drawCircle(180, 180, 39, FOREGROUND_COLOR, 3);
}


void WatchyJarvis::drawCircle(int16_t x0, int16_t y0, int16_t r, 
    uint16_t color, uint8_t width){
    for(int i=0; i < width; i++){
        display.drawCircle(x0, y0, r-i, color);
    }
}