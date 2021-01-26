#include "Watchy_Step.h"

#define DARKMODE            false
#define FOREGROUND_COLOR    (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)
#define BACKGROUND_COLOR    (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
#define GRAPH_COLOR         FOREGROUND_COLOR
#define BATTERY_OFFSET      0.25


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
    display.setCursor(10, 40);
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
    display.setCursor(150, 20);
    //String day = dayShortStr(weekday(currentTime));
    String dayOfWeek = dayShortStr(currentTime.Wday);
    display.println(dayOfWeek);

    display.setCursor(150, 45);
    if(currentTime.Day < 10){
      display.print("0");      
    }     
    display.println(currentTime.Day);
}


void WatchyStep::drawSteps(){
    display.setFont(&FreeSans12pt7b);

    // We sleep for 1 minute so it could happen that we miss one minute.
    // To ensure that everything is fine we reset 2 minutes...
    if(currentTime.Hour == 0 && currentTime.Minute <= 1){
        sensor.resetStepCounter();
    }

    uint32_t stepCount = sensor.getCounter();

    float max_steps = -1;
    for(int i=0; i < 24; i++){
        max_steps = max(float(steps_hours[i]), max_steps);
    }
    
    // Now lets draw the graph
    if(currentTime.Minute <= 1){
        steps_hours[currentTime.Hour] = stepCount;
        if(currentTime.Hour > 0){
            steps_hours[currentTime.Hour] -= steps_hours[currentTime.Hour-1];
        }
    }


    //display.drawLine(5, 59, 195, 59, GRAPH_COLOR);
    //display.drawLine(5, 60, 195, 60, GRAPH_COLOR);

    display.drawLine(5, 159, 195, 159, GRAPH_COLOR);
    display.drawLine(5, 160, 195, 160, GRAPH_COLOR);

    for(int h=0; h<24; h++){
        //display.setCursor(35, 70 + h*20);
        //display.println(steps_hours[h]);

        // height = 90
        // width = 192 for graph
        // so 192/24 = 8 pixel for each hour
        int8_t relative_steps = 100 * float(steps_hours[h]) / max_steps;
        display.drawLine(4 + h*8 + 2, 160, 4 + h*8 + 2, 160-relative_steps, GRAPH_COLOR);
        display.drawLine(4 + h*8 + 3, 160, 4 + h*8 + 3, 160-relative_steps, GRAPH_COLOR);
        display.drawLine(4 + h*8 + 4, 160, 4 + h*8 + 4, 160-relative_steps, GRAPH_COLOR);
        display.drawLine(4 + h*8 + 5, 160, 4 + h*8 + 5, 160-relative_steps, GRAPH_COLOR);
    }


    // Show number of steps as text and bitmap
    display.drawBitmap(5, 170, steps, 19, 23, FOREGROUND_COLOR);
    display.setCursor(35, 192);
    display.println(stepCount);
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
        
    display.drawBitmap(160, 170, battery, 37, 21, FOREGROUND_COLOR);
    display.fillRect(160+5, 170+5, 27, 11, BACKGROUND_COLOR);//clear battery segments
    for(int8_t batterySegments = 0; batterySegments < batteryLevel; batterySegments++){
        display.fillRect(160 + (batterySegments * 9), 170+5, 7, 11, FOREGROUND_COLOR);
    }

    /*display.setFont(&FreeSans12pt7b);
    display.setCursor(130, 195);
    display.print(percentage);
    display.println("%");*/

    /*
    display.setFont(&FreeSans12pt7b);
    display.setCursor(125, 125);
    display.print(percentage);
    display.println("%");

    display.setFont(&FreeSans12pt7b);
    display.setCursor(125, 150);
    display.print(voltage);
    display.println("V");*/
}
