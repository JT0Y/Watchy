#include "Watchy_BCD.h"


// For more fonts look here:
// https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
#define FONT    FreeMono12pt7b


WatchyBCD::WatchyBCD(){

}


void WatchyBCD::handleButtonPress(){
    WatchyBase::handleButtonPress();
    
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if(IS_DOUBLE_TAP){
        // NOP
    }
}


void WatchyBCD::drawWatchFace(){
    WatchyBase::drawWatchFace();
    if(show_mqqt_data){
        return;
    }

    bool rtc_alarm = wakeup_reason == ESP_SLEEP_WAKEUP_EXT0;
    if(rtc_alarm && currentTime.Minute == 0 && currentTime.Hour == 0){
        sensor.resetStepCounter();
    }
    
    display.setFont(&FONT);
    display.setTextColor(FOREGROUND_COLOR);

    for(uint8_t i=0; i<4; i++){
        display.drawRoundRect(0+i, 0+i, 196, 196, 4, FOREGROUND_COLOR);
    }
    display.setCursor(50, 30);
    display.println("10b");
    display.setCursor(130, 30);
    display.println("01b");

    uint32_t steps = sensor.getCounter();
    uint8_t bat = getBattery();
    bat = min((uint8_t) 99, bat);

    printBCD("M", 40, currentTime.Month);
    printBCD("D", 60, currentTime.Day);

    printBCD("h", 90, currentTime.Hour);
    printBCD("m", 110, currentTime.Minute);

    printBCD("B", 140, bat);
    printBCD("S", 170, steps / 1000);
}


void WatchyBCD::printBCD(String s, uint16_t y, uint16_t value){
    display.setCursor(15, y+13);
    display.println(s);
    printBinary(40, y, (uint8_t) value / 10, 4);
    printBinary(120, y, (uint8_t) value % 10, 4);
}

void WatchyBCD::printBinary(uint16_t x, uint16_t y, uint8_t value, uint8_t n){
    if(n <= 0){
        return;
    }

    uint8_t gap = 2;
    uint8_t size = 15;
    uint8_t x_pos = x + (4-n)*(size+gap);
    if(value % 2 == 0){
        display.drawRoundRect(x_pos, y, size, size, 3, FOREGROUND_COLOR);
    } else {
        display.fillRoundRect(x_pos, y, size, size, 3, FOREGROUND_COLOR);
    }

    printBinary(x, y, value / 2, n-1);
}