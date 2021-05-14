#include "Watchy_Dot.h"


#define FONT                Himalaya_40020pt7b
#define WHITE_TEXT_SIZE     3

WatchyDot::WatchyDot(){

}


void WatchyDot::handleButtonPress(){
    WatchyBase::handleButtonPress();

    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if(IS_DOUBLE_TAP){
        dark_mode = dark_mode ? false : true;
        RTC.read(currentTime);
        showWatchFace(false);
        return;
    }
}


void WatchyDot::drawWatchFace(){
    WatchyBase::drawWatchFace();
    if(watchFaceDisabled()){
        return;
    }

    drawBattery();
    drawDate();
    drawSteps();
    drawTime();
}


void WatchyDot::printCentered(uint16_t x, uint16_t y, String text){
    int16_t  x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 40, 100, &x1, &y1, &w, &h);

    display.setTextColor(BACKGROUND_COLOR);
    for(int i=-WHITE_TEXT_SIZE;i<WHITE_TEXT_SIZE+1;i++){
        for(int j=-WHITE_TEXT_SIZE;j<WHITE_TEXT_SIZE+1;j++){
            display.setCursor(x-w/2+i, y+h/2+j);
            display.println(text);
        }
    }

    display.setTextColor(FOREGROUND_COLOR);
    display.setCursor(x-w/2, y+h/2);
    display.println(text);
    display.setCursor(x-w/2-1, y+h/2);
    display.println(text);
    display.setCursor(x-w/2+1, y+h/2);
    display.println(text);
    display.setCursor(x-w/2, y+h/2+1);
    display.println(text);
    display.setCursor(x-w/2, y+h/2-1);
    display.println(text);
}


float M(float theta){
    return max(abs(cos(theta)), abs(sin(theta)));
}


void WatchyDot::drawTime(){
    // See also https://math.stackexchange.com/questions/2468060/find-x-y-coordinates-of-a-square-given-an-angle-alpha
    int theHour = currentTime.Hour;
    int theMinute = currentTime.Minute;

    // Hour hand
    int s = 80;
    float theta = ((theHour%12)*60 + theMinute) * 360 / 720;
    theta = (int)(theta-90) % 360;
    theta = theta * PI / 180.0;

    int x = 100 + (int)(cos(theta) * s / M(theta));
    int y = 100 + (int)(sin(theta) * s / M(theta));

    display.fillCircle(x, y, 14, FOREGROUND_COLOR);
    display.fillCircle(x, y, 12, BACKGROUND_COLOR);

    // Minute hand
    theta = theMinute * 360 / 60;
    theta = (int)(theta-90) % 360;
    theta = theta * PI / 180.0;
    x = 100 + (int)(cos(theta) * s / M(theta));
    y = 100 + (int)(sin(theta) * s / M(theta));
    display.fillCircle(x, y, 12, BACKGROUND_COLOR);
    display.fillCircle(x, y, 9, FOREGROUND_COLOR);

    // Alarm timer
    if(alarm_timer > 0){
        s = 75;
        theta = ((theMinute + alarm_timer) % 60) * 360 / 60;
        theta = (int)(theta-90) % 360;
        theta = theta * PI / 180.0;
        x = 100 + (int)(cos(theta) * s / M(theta));
        y = 100 + (int)(sin(theta) * s / M(theta));
        printCentered(x, y, String(alarm_timer));
    }
}


void WatchyDot::drawDate(){
    display.setFont(&FONT);
    display.setTextColor(FOREGROUND_COLOR);

    // Day
    String dayStr = String(currentTime.Day);
    dayStr = currentTime.Day < 10 ? "0" + dayStr : dayStr;
    String date = dayShortStr(currentTime.Wday);
    date += "  " + String(dayStr);
    printCentered(100, 65, date);
}


void WatchyDot::drawBattery(){
    int8_t bat = getBattery();
    bat = bat >= 100 ? 99 : bat;
    for(int x=0; x < 200; x++){
        for(int y=200; y > 200-bat*2; y--){
            drawPixel(x, y, DARK_GREY);
        }
    }
}


void WatchyDot::drawSteps(){
    display.setFont(&FONT);
    display.setTextColor(FOREGROUND_COLOR);

    bool rtc_alarm = wakeup_reason == ESP_SLEEP_WAKEUP_EXT0;

    // Whenever we have a new hour, we can restart our step counting.
    // But only if its an rtc alarm - ignore button press etc.
    if(rtc_alarm && currentTime.Minute == 0 && currentTime.Hour == 0){
        sensor.resetStepCounter();
    }

    uint32_t steps = sensor.getCounter();
    String stepStr = String(steps);

    printCentered(100, 145, stepStr);
}