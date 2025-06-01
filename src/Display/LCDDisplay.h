#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

class LCDDisplay {
public:
    LCDDisplay(int sda_pin, int scl_pin, uint8_t address = 0x27);
    bool begin();
    void displayMessage(String line1, String line2);
    void displayIMUData(float roll, float pitch, float yaw);
    void displayGPSData(float latitude, float longitude);
    void displayCompassData(float heading, String cardinalDir);

private:
    LiquidCrystal_I2C lcd;
    int sdaPin;
    int sclPin;
    uint8_t i2cAddress;
};

#endif
