#include "Watchy_Step.h"


// Color settings
#define DARKMODE            false
#define FOREGROUND_COLOR    (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)
#define BACKGROUND_COLOR    (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
#define GRAPH_COLOR         FOREGROUND_COLOR
#define BATTERY_OFFSET      0.25

// For more fonts look here:
// https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
#define FONT_LARGE_BOLD  FreeSansBold24pt7b
#define FONT_MEDUM_BOLD  FreeSansBold12pt7b
#define FONT_MEDUM       FreeSans12pt7b
#define FONT_SMALL_BOLD  FreeSansBold9pt7b
#define FONT_SMALL       FreeSans9pt7b

// Store step in RTC RAM, otherwise we loose information between 
// different interrupts
RTC_DATA_ATTR int steps_hours[24] = {0};
RTC_DATA_ATTR int last_step_count = 0;
RTC_DATA_ATTR int rotation = 0;
RTC_DATA_ATTR bool print_date = true;

WatchyStep::WatchyStep(){

}

void WatchyStep::init(){
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause(); //get wake up reason
    Wire.begin(SDA, SCL); //init i2c

    switch (wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0: //RTC Alarm
            RTC.alarm(ALARM_2); //resets the alarm flag in the RTC
            if(guiState == WATCHFACE_STATE){
                RTC.read(currentTime);
                showWatchFace(true); //partial updates on tick
            }else{
            //
            }
            break;
        case ESP_SLEEP_WAKEUP_EXT1: //button Press
            handleButtonPress();
            break;
        default: //reset
            _rtcConfig();
            _bmaConfig();
            showWatchFace(false); //full update on reset
            break;
    }
    deepSleep();
}


void WatchyStep::handleButtonPress(){
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
    if (wakeupBit & BACK_BTN_MASK && guiState == WATCHFACE_STATE){
        rotation = rotation == 2 ? 0 : 2;
        RTC.read(currentTime);
        showWatchFace(false);
        return;
    }

    if(wakeupBit & UP_BTN_MASK && guiState == WATCHFACE_STATE){
        print_date = print_date ? false : true;
        RTC.read(currentTime);
        showWatchFace(true);
        return;
    }
    
    Watchy::handleButtonPress();
}


void WatchyStep::drawWatchFace(){
    display.setRotation(rotation);
    display.fillScreen(BACKGROUND_COLOR);
    display.setTextColor(FOREGROUND_COLOR);
    drawTime();
    drawSteps();
    if(print_date){
        drawDate();
    } else {
        drawBattery();
    }
}


void WatchyStep::drawTime(){
    display.setFont(&FONT_LARGE_BOLD);
    display.setCursor(12, 40);
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

    display.setFont(&FONT_MEDUM);
    display.setCursor(145, 23);
    String dayOfWeek = dayShortStr(currentTime.Wday);
    display.println(dayOfWeek);
    display.setCursor(145, 43);
    display.println(currentTime.Day);
}


void WatchyStep::drawBattery(){
    float voltage = getBatteryVoltage() + BATTERY_OFFSET;
    
    int8_t percentage = 2808.3808 * pow(voltage, 4) 
                        - 43560.9157 * pow(voltage, 3) 
                        + 252848.5888 * pow(voltage, 2) 
                        - 650767.4615 * voltage 
                        + 626532.5703;
    percentage = min((int8_t) 99, percentage);
    percentage = max((int8_t) 0, percentage);
    
    display.drawBitmap(150, 15, battery, 37, 21, FOREGROUND_COLOR);
    display.setFont(&FONT_SMALL_BOLD);
    display.setCursor(157, 31);
    if(percentage < 10) {
        display.print("0");
    }
    display.println(percentage);
}


float WatchyStep::getMaxSteps(){
    float max_steps = -1;
    for(int i=0; i < 24; i++){
        max_steps = max(float(steps_hours[i]), max_steps);
    }
    return max_steps;
}


void WatchyStep::drawSteps(){   
    int8_t max_height = 70;

    // Bottom line
    //display.drawLine(5, 139, 195, 139, FOREGROUND_COLOR);
    display.drawLine(5, 140, 195, 140, FOREGROUND_COLOR);
    //display.drawLine(5, 141, 195, 141, FOREGROUND_COLOR);
    //display.drawLine(5, 141, 195, 142, FOREGROUND_COLOR);

    // Top line
    //display.drawLine(5, 139-max_height, 195, 139-max_height, FOREGROUND_COLOR);
    display.drawLine(5, 140-max_height, 195, 140-max_height, FOREGROUND_COLOR);
    //display.drawLine(5, 141-max_height, 195, 141-max_height, FOREGROUND_COLOR);

    // Text of bottom line
    display.setFont(&FONT_SMALL);
    int16_t  x1, y1;
    uint16_t width, height;
    display.getTextBounds("12", 55, 195, &x1, &y1, &width, &height);
    display.setCursor(4+12*8 + 2-int(width/2), 160);
    display.println("12");
    display.setCursor(4+6*8 + 2-int(width/2), 160);
    display.println("06");
    display.setCursor(4+18*8 + 2-int(width/2), 160);
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
    
    // Print max steps for y axis
    uint32_t max_steps = getMaxSteps();
    display.setCursor(12, 140-max_height-5);
    display.print(max_steps);
    display.println(" steps");

    for(int h=0; h < 24; h++){
        // Clean lines for current hour
        uint32_t relative_steps = max_height * float(steps_hours[h]) / float(max_steps+1); // Numerical stability
        display.drawLine(4+h*8 + 2, 140, 4+h*8 + 2, 140-relative_steps, GRAPH_COLOR);
        display.drawLine(4+h*8 + 3, 140, 4+h*8 + 3, 140-relative_steps, GRAPH_COLOR);
        display.drawLine(4+h*8 + 4, 140, 4+h*8 + 4, 140-relative_steps, GRAPH_COLOR);
        display.drawLine(4+h*8 + 5, 140, 4+h*8 + 5, 140-relative_steps, GRAPH_COLOR);
    }

    // Show current position
    display.drawLine(4+pos*8 + 2, 140, 4+pos*8 + 2, 146, FOREGROUND_COLOR);
    display.drawLine(4+pos*8 + 3, 140, 4+pos*8 + 3, 146, FOREGROUND_COLOR);
    display.drawLine(4+pos*8 + 4, 140, 4+pos*8 + 4, 146, FOREGROUND_COLOR);
    display.drawLine(4+pos*8 + 5, 140, 4+pos*8 + 5, 146, FOREGROUND_COLOR);

    // Show number of steps as text and bitmap
    display.fillRect(0, 170, 200, 200, FOREGROUND_COLOR);
    display.setTextColor(BACKGROUND_COLOR);
    display.setFont(&FONT_MEDUM_BOLD);    
    display.getTextBounds(String(step_count), 55, 195, &x1, &y1, &width, &height);
    int8_t bitmap_pos = int(200-width)/2;
    display.drawBitmap(bitmap_pos-15, 174, steps, 19, 23, BACKGROUND_COLOR);
    display.setCursor(bitmap_pos+15, 192);
    display.println(step_count);
    display.setTextColor(FOREGROUND_COLOR);
}


void WatchyStep::_rtcConfig(){
    //https://github.com/JChristensen/DS3232RTC
    RTC.squareWave(SQWAVE_NONE); //disable square wave output
    //RTC.set(compileTime()); //set RTC time to compile time
    RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 0); //alarm wakes up Watchy every minute
    RTC.alarmInterrupt(ALARM_2, true); //enable alarm interrupt
    RTC.read(currentTime);
}


void WatchyStep::_bmaConfig(){
 
    if (sensor.begin(_readRegister, _writeRegister, delay) == false) {
        //fail to init BMA
        return;
    }

    // Accel parameter structure
    Acfg cfg;
    /*!
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    /*!
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
    cfg.range = BMA4_ACCEL_RANGE_2G;
    /*!
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /*! Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    // Configure the BMA423 accelerometer
    sensor.setAccelConfig(cfg);

    // Enable BMA423 accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    sensor.enableAccel();

    struct bma4_int_pin_config config ;
    config.edge_ctrl = BMA4_LEVEL_TRIGGER;
    config.lvl = BMA4_ACTIVE_HIGH;
    config.od = BMA4_PUSH_PULL;
    config.output_en = BMA4_OUTPUT_ENABLE;
    config.input_en = BMA4_INPUT_DISABLE;
    // The correct trigger interrupt needs to be configured as needed
    sensor.setINTPinConfig(config, BMA4_INTR1_MAP);

    struct bma423_axes_remap remap_data;
    remap_data.x_axis = 1;
    remap_data.x_axis_sign = 0;
    remap_data.y_axis = 0;
    remap_data.y_axis_sign = 0;
    remap_data.z_axis  = 2;
    remap_data.z_axis_sign  = 0;
    // Need to raise the wrist function, need to set the correct axis
    sensor.setRemapAxes(&remap_data);

    // Enable BMA423 isStepCounter feature
    sensor.enableFeature(BMA423_STEP_CNTR, true);
    // Enable BMA423 isTilt feature
    sensor.enableFeature(BMA423_TILT, true);
    // Enable BMA423 isDoubleClick feature
    sensor.enableFeature(BMA423_WAKEUP, true);

    // Reset steps
    sensor.resetStepCounter();

    // Turn on feature interrupt
    sensor.enableStepCountInterrupt();
    sensor.enableTiltInterrupt();
    // It corresponds to isDoubleClick interrupt
    sensor.enableWakeupInterrupt();  
}


uint16_t WatchyStep::_readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)address, (uint8_t)len);
    uint8_t i = 0;
    while (Wire.available()) {
        data[i++] = Wire.read();
    }
    return 0;
}

uint16_t WatchyStep::_writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data, len);
    return (0 !=  Wire.endTransmission());
}