#include "Watchy_Base.h"


// Store in RTC RAM, otherwise we loose information between different interrupts
RTC_DATA_ATTR uint8_t rotation = 0;
RTC_DATA_ATTR time_t lastDoubleTap;
RTC_DATA_ATTR bool dark_mode = false;

WatchyBase::WatchyBase(){

}


void WatchyBase::init(){
    wakeup_reason = esp_sleep_get_wakeup_cause(); //get wake up reason
    Wire.begin(SDA, SCL); //init i2c

    switch (wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0: //RTC Alarm
            RTC.alarm(ALARM_2); //resets the alarm flag in the RTC
            if(guiState == WATCHFACE_STATE){
                RTC.read(currentTime);
                showWatchFace(true); //partial updates on tick
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

void WatchyBase::deepSleep(){
  esp_sleep_enable_ext0_wakeup(RTC_PIN, 0); //enable deep sleep wake on RTC interrupt
  esp_sleep_enable_ext1_wakeup(EXT_INT_MASK, ESP_EXT1_WAKEUP_ANY_HIGH); //enable deep sleep wake on button press
  esp_deep_sleep_start();
}


void WatchyBase::handleButtonPress(){
    uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();

    if (wakeupBit & ACC_INT_MASK && guiState == WATCHFACE_STATE){
        // Should not occur
        /*RTC.read(currentTime);
        time_t now = makeTime(currentTime);
        bool gestureDetected = (now - lastDoubleTap) < DOUBLE_TAP_TIME;
        lastDoubleTap = now;
        
        pinMode(VIB_MOTOR_PIN, OUTPUT);
        if(gestureDetected){
            digitalWrite(VIB_MOTOR_PIN, true);
            delay(1000);
            digitalWrite(VIB_MOTOR_PIN, false);
            
            openDoor();
        }

        while(!sensor.getINT()){
            // Wait until interrupt is cleared.
            // Otherwise it will fire again and again.
        }*/
        return;
    }

    if (wakeupBit & BACK_BTN_MASK && guiState == WATCHFACE_STATE){
        rotation = rotation == 2 ? 0 : 2;
        RTC.read(currentTime);
        showWatchFace(false);
        return;
    }

    if(wakeupBit & UP_BTN_MASK && guiState == WATCHFACE_STATE){
        // To be defined by the watch face (child)
    }

    if(wakeupBit & DOWN_BTN_MASK && guiState == WATCHFACE_STATE){
        //RTC.read(currentTime);
        //vibTime();
        //return;
        pinMode(VIB_MOTOR_PIN, OUTPUT);
        digitalWrite(VIB_MOTOR_PIN, true);
        delay(150);
        digitalWrite(VIB_MOTOR_PIN, false);
        
        if(openDoor()){
            digitalWrite(VIB_MOTOR_PIN, true);
            delay(150);
            digitalWrite(VIB_MOTOR_PIN, false);
        }
        return;
    }
    
    Watchy::handleButtonPress();
}



bool WatchyBase::connectWiFi(){

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    int8_t retries = 20;
    while (WiFi.status() != WL_CONNECTED) {
        if(retries < 0){
            break;
        }

        delay(100);
        retries--;
    }

    WIFI_CONFIGURED = WiFi.status() == WL_CONNECTED;
    return WIFI_CONFIGURED;
}

void WatchyBase::disconnectWiFi(){
    WIFI_CONFIGURED=false;
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    btStop();
}


// https://github.com/espressif/arduino-esp32/issues/3659
bool WatchyBase::openDoor(){
    if(!connectWiFi()){
        return false;
    }

    WiFiClient espClient;
    PubSubClient client(espClient);
    client.setServer(MQTT_BROKER, 1883);
    int8_t retries = 20;
    while(!client.connected()){
        if(retries < 0){
            break;
        }

        delay(100);
        retries--;
    }

    if(client.connect("WatchyDavid")){
        client.publish(MQTT_TOPIC, MQTT_PAYLOAD);
    }
    

    disconnectWiFi();
    return true;
}


void WatchyBase::vibTime(){
    pinMode(VIB_MOTOR_PIN, OUTPUT);

    // Hours
    uint8_t h_5_buzz = currentTime.Hour/5;
    for(int8_t i=0; i < h_5_buzz; i++){
        digitalWrite(VIB_MOTOR_PIN, true);
        delay(400);
        digitalWrite(VIB_MOTOR_PIN, false);
        delay(400);
    }

    uint8_t h_1_buzz = currentTime.Hour - h_5_buzz * 5;
    for(int8_t i=0; i < h_1_buzz; i++){
        digitalWrite(VIB_MOTOR_PIN, true);
        delay(200);
        digitalWrite(VIB_MOTOR_PIN, false);
        delay(200);
    }
    
    // 10-Minutes
    delay(2000);

    uint8_t m_10_buzz = currentTime.Minute / 10;
    for(int8_t i=0; i < m_10_buzz; i++){
        digitalWrite(VIB_MOTOR_PIN, true);
        delay(200);
        digitalWrite(VIB_MOTOR_PIN, false);
        delay(200);
    }
}

void WatchyBase::drawWatchFace(){
    display.setRotation(rotation);
    display.fillScreen(BACKGROUND_COLOR);
    display.setTextColor(FOREGROUND_COLOR);
}


uint8_t WatchyBase::getBattery(){
    float voltage = getBatteryVoltage() + BATTERY_OFFSET;
    
    uint8_t percentage = 2808.3808 * pow(voltage, 4) 
                        - 43560.9157 * pow(voltage, 3) 
                        + 252848.5888 * pow(voltage, 2) 
                        - 650767.4615 * voltage 
                        + 626532.5703;
    percentage = min((uint8_t) 100, percentage);
    percentage = max((uint8_t) 0, percentage);
    return percentage;
}



void WatchyBase::_rtcConfig(){
    //https://github.com/JChristensen/DS3232RTC
    RTC.squareWave(SQWAVE_NONE); //disable square wave output
    //RTC.set(compileTime()); //set RTC time to compile time
    RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0, 0); //alarm wakes up Watchy every minute
    RTC.alarmInterrupt(ALARM_2, true); //enable alarm interrupt
    RTC.read(currentTime);
}


void WatchyBase::_bmaConfig(){
 
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
    remap_data.x_axis_sign = 0xFF;
    remap_data.y_axis = 0;
    remap_data.y_axis_sign = 0xFF;
    remap_data.z_axis = 2;
    remap_data.z_axis_sign = 0xFF;
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
    //sensor.enableStepCountInterrupt();
    //sensor.enableTiltInterrupt();
    // It corresponds to isDoubleClick interrupt
    //sensor.enableWakeupInterrupt();  
}


uint16_t WatchyBase::_readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len)
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

uint16_t WatchyBase::_writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data, len);
    return (0 !=  Wire.endTransmission());
}