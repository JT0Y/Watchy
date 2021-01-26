#include "Watchy_David.h"

#define DARKMODE false
#define FOREGROUND_COLOR (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)
#define BACKGROUND_COLOR (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
#define BATTERY_OFFSET 0.25

const uint8_t BATTERY_SEGMENT_WIDTH = 7;
const uint8_t BATTERY_SEGMENT_HEIGHT = 11;
const uint8_t BATTERY_SEGMENT_SPACING = 9;
const uint8_t WEATHER_ICON_WIDTH = 48;
const uint8_t WEATHER_ICON_HEIGHT = 32;


WatchyDavid::WatchyDavid(){} //constructor


void WatchyDavid::drawWatchFace(){
    display.fillScreen(BACKGROUND_COLOR);
    display.setTextColor(FOREGROUND_COLOR);
    drawTime();
    drawDate();
    drawSteps();
    drawBattery();

    for(int i=0; i<3; i++){
      display.drawLine(110+i, 75, 110+i, 155, FOREGROUND_COLOR);
    }
    
    
    for(int i=0; i<3; i++){
      display.drawLine(10, 65+i, 190, 65+i, FOREGROUND_COLOR);
    }

    for(int i=0; i<3; i++){
      display.drawLine(10, 160+i, 190, 160+i, FOREGROUND_COLOR);
    }
}


void WatchyDavid::drawTime(){
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setCursor(5, 53+5);
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


void WatchyDavid::drawDate(){
    int16_t  x1, y1;
    uint16_t w, h;

    display.setFont(&FreeSans12pt7b);
    String month = monthShortStr(currentTime.Month);
    display.getTextBounds(month, 60, 120, &x1, &y1, &w, &h);
    display.setCursor(88 - w, 110);
    display.println(month);

    display.setFont(&FreeSans18pt7b);
    display.setCursor(15, 110);
    if(currentTime.Day < 10){
      display.print("0");      
    }     
    display.println(currentTime.Day);
    display.setCursor(15, 140);
    display.println(currentTime.Year + YEAR_OFFSET);// offset from 1970, since year is stored in uint8_t
}


void WatchyDavid::drawSteps(){
    display.setFont(&FreeSans18pt7b);
    uint32_t stepCount = sensor.getCounter();
    int16_t  x1, y1;
    uint16_t w, h;
    
    display.getTextBounds(String(stepCount), 55, 195, &x1, &y1, &w, &h);
    int8_t pos = int(200-w)/2;
    display.drawBitmap(pos-15, 174, steps, 19, 23, FOREGROUND_COLOR);
    display.setCursor(pos+15, 195);
    display.println(stepCount);
}


void WatchyDavid::drawBattery(){
    float voltage = getBatteryVoltage() + BATTERY_OFFSET;
    int8_t percentage = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;
    int8_t batteryLevel = percentage > 80 ? 4 :
                          percentage > 65 ? 3 :
                          percentage > 50 ? 2 :
                          percentage > 35 ? 1 : 0;
    display.drawBitmap(125, 78, battery, 37, 21, FOREGROUND_COLOR);
    display.fillRect(125+5, 83, 27, BATTERY_SEGMENT_HEIGHT, BACKGROUND_COLOR);//clear battery segments
    for(int8_t batterySegments = 0; batterySegments < batteryLevel; batterySegments++){
        display.fillRect(125 + (batterySegments * BATTERY_SEGMENT_SPACING), 83, BATTERY_SEGMENT_WIDTH, BATTERY_SEGMENT_HEIGHT, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    }

    display.setFont(&FreeSans12pt7b);
    display.setCursor(125, 125);
    display.print(percentage);
    display.println("%");

    display.setFont(&FreeSans12pt7b);
    display.setCursor(125, 150);
    display.print(voltage);
    display.println("V");
}
