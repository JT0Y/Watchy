#include "Watchy_Step.h"

#define DARKMODE            false
#define FOREGROUND_COLOR    (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)
#define BACKGROUND_COLOR    (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
#define GRAPH_COLOR         FOREGROUND_COLOR
#define BATTERY_OFFSET      0.25


// They are stored in RTC RAM - Thererfore they are not part of the class.
RTC_DATA_ATTR int steps_hours[24] = {0};
RTC_DATA_ATTR int last_step_count = 0;

WatchyStep::WatchyStep(){} //constructor


void WatchyStep::drawWatchFace(){
    display.fillScreen(BACKGROUND_COLOR);
    display.setTextColor(FOREGROUND_COLOR);
    drawTime();
    drawDate();
    drawSteps();
    drawBattery();
}


void WatchyStep::drawTime(){
    display.setFont(&FreeSans24pt7b);
    display.setCursor(15, 40);
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


void WatchyStep::drawDate(){
    int16_t  x1, y1;
    uint16_t w, h;

    display.setFont(&FreeSans12pt7b);
    display.setCursor(15, 65);
    String dayOfWeek = dayShortStr(currentTime.Wday);
    display.print(dayOfWeek);
    display.print(" ");
    display.println(currentTime.Day);
}


float WatchyStep::getMaxSteps(){
    float max_steps = -1;
    for(int i=0; i < 24; i++){
        max_steps = max(float(steps_hours[i]), max_steps);
    }
    return max_steps;
}


void WatchyStep::drawSteps(){   
    display.setFont(&FreeSans12pt7b);
    display.drawLine(5, 139, 195, 139, GRAPH_COLOR);
    display.drawLine(5, 140, 195, 140, GRAPH_COLOR);
    display.drawLine(5, 141, 195, 141, GRAPH_COLOR);

    int16_t  x1, y1;
    uint16_t width, height;
    
    display.getTextBounds("12", 55, 195, &x1, &y1, &width, &height);

    display.setCursor(12*8+4-int(width/2), 165);
    display.println("12");
    display.setCursor(6*8+4-int(width/2), 165);
    display.println("06");
    display.setCursor(18*8+4-int(width/2), 165);
    display.println("18");

    // We sleep for 1 minute so it could happen that we miss one minute.
    // To ensure that everything is fine we reset 2 minutes...
    if(currentTime.Hour == 0 && currentTime.Minute <= 1){
        sensor.resetStepCounter();
        last_step_count = 0;
    }
    
    // Lets write the absolute steps for this hour
    uint32_t step_count = sensor.getCounter();
    uint8_t pos = currentTime.Hour > 0 ? currentTime.Hour-1 : 23;
    steps_hours[pos] = step_count - last_step_count;
    
    if(currentTime.Minute <= 1){
        last_step_count = step_count;
    }    
    
    float max_steps = getMaxSteps();
    for(int h=0; h < 24; h++){
        // width = 192 for graph
        // so 192/24 = 8 pixel for each hour
        int8_t relative_steps = 70 * float(steps_hours[h]) / max_steps;        
        display.drawLine(h*8 + 2, 140, h*8 + 2, 140-relative_steps, GRAPH_COLOR);
        display.drawLine(h*8 + 3, 140, h*8 + 3, 140-relative_steps, GRAPH_COLOR);
        display.drawLine(h*8 + 4, 140, h*8 + 4, 140-relative_steps, GRAPH_COLOR);
        display.drawLine(h*8 + 5, 140, h*8 + 5, 140-relative_steps, GRAPH_COLOR);
    }

    // Show current position
    display.drawLine(pos*8 + 3, 145, pos*8 + 3, 136, GRAPH_COLOR);
    display.drawLine(pos*8 + 4, 145, pos*8 + 4, 136, GRAPH_COLOR);
    display.drawLine(pos*8 + 5, 145, pos*8 + 5, 136, GRAPH_COLOR);

    // Show number of steps as text and bitmap
    display.setFont(&FreeSans18pt7b);    
    display.getTextBounds(String(step_count), 55, 195, &x1, &y1, &width, &height);
    int8_t bitmap_pos = int(200-width)/2;
    display.drawBitmap(bitmap_pos-15, 174, steps, 19, 23, FOREGROUND_COLOR);
    display.setCursor(bitmap_pos+15, 195);
    display.println(step_count);
}


void WatchyStep::drawBattery(){
    float voltage = getBatteryVoltage() + BATTERY_OFFSET;
    
    int8_t percentage = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;
    percentage = min((int8_t) 100, percentage);
    percentage = max((int8_t) 0, percentage);
    
    int8_t batteryLevel = percentage > 80 ? 4 :
                          percentage > 65 ? 3 :
                          percentage > 50 ? 2 :
                          percentage > 35 ? 1 : 0;
        
    display.drawBitmap(147, 15, battery, 37, 21, FOREGROUND_COLOR);
    display.fillRect(147+5, 15+5, 27, 11, BACKGROUND_COLOR);//clear battery segments
    for(int8_t batterySegments = 0; batterySegments < batteryLevel; batterySegments++){
        display.fillRect(147 + (batterySegments * 9), 15+5, 7, 11, FOREGROUND_COLOR);
    }
}
