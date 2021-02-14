#include "Watchy_BCD.h"


// For more fonts look here:
// https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
#define FONT    FreeMonoBold12pt7b


WatchyBCD::WatchyBCD(){
    
}


void WatchyBCD::handleButtonPress(){
    WatchyBase::handleButtonPress();
    
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if(IS_DOUBLE_TAP){
        dark_mode = dark_mode ? false : true;
        RTC.read(currentTime);
        showWatchFace(false);
        return;
    }
}


void WatchyBCD::drawWatchFace(){
    WatchyBase::drawWatchFace();
    if(show_mqqt_data){
        return;
    }

    display.drawBitmap(0, 0, pcb, 200, 200, FOREGROUND_COLOR);

    bool rtc_alarm = wakeup_reason == ESP_SLEEP_WAKEUP_EXT0;
    if(rtc_alarm && currentTime.Minute == 0 && currentTime.Hour == 0){
        sensor.resetStepCounter();
    }
    
    display.setFont(&FONT);
    display.setTextColor(FOREGROUND_COLOR);

    display.fillRect(45, 13, 50, 21, BACKGROUND_COLOR);
    display.setCursor(50, 28);
    display.println("10b");

    display.fillRect(125, 13, 50, 21, BACKGROUND_COLOR);
    display.setCursor(130, 28);
    display.println("01b");

    uint32_t steps = sensor.getCounter();
    uint8_t bat = getBattery();
    bat = min((uint8_t) 99, bat);

    printBCD("h", 40, currentTime.Hour);
    printBCD("m", 60, currentTime.Minute);

    printBCD("M", 90, currentTime.Month);
    printBCD("D", 110, currentTime.Day);

    printBCD("B", 140, bat);
    printBCD("S", 170, steps / 100);
}


void WatchyBCD::printBCD(String s, uint16_t y, uint16_t value){
    display.fillRect(10, y-5, 22, 22, BACKGROUND_COLOR);
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
    uint8_t x_pos = x + (n-1)*(size+gap);

    display.fillRoundRect(x_pos-gap, y-gap-1, size+2*gap, size+2*gap+2, 3, BACKGROUND_COLOR);
    if(value % 2 == 0){
        display.drawRoundRect(x_pos, y, size, size, 3, FOREGROUND_COLOR);
        display.drawRoundRect(x_pos+1, y+1, size-2, size-2, 3, FOREGROUND_COLOR);
        display.fillRoundRect(x_pos+2, y+2, size-4, size-4, 3, BACKGROUND_COLOR);
    } else {
        display.fillRoundRect(x_pos, y, size, size, 3, FOREGROUND_COLOR);
    }

    printBinary(x, y, value / 2, n-1);
}