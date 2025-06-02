#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Wire.h>
#include <Arduino.h>

class LCDDisplay {
public:
    LCDDisplay(int sda_pin, int scl_pin, uint8_t address = 0x27);
    bool begin();
    void displayMessage(String line1, String line2);
    void displayIMUData(float roll, float pitch, float yaw);
    void displayGPSData(float latitude, float longitude);
    void displayCompassData(float heading, String cardinalDir);
    void clear();
    void setCursor(uint8_t col, uint8_t row);
    void print(String text);
    void backlight(bool state);

private:
    int sdaPin;
    int sclPin;
    uint8_t i2cAddress;
    uint8_t backlightState;
    
    // LCD Commands
    static const uint8_t LCD_CLEARDISPLAY = 0x01;
    static const uint8_t LCD_RETURNHOME = 0x02;
    static const uint8_t LCD_ENTRYMODESET = 0x04;
    static const uint8_t LCD_DISPLAYCONTROL = 0x08;
    static const uint8_t LCD_CURSORSHIFT = 0x10;
    static const uint8_t LCD_FUNCTIONSET = 0x20;
    static const uint8_t LCD_SETCGRAMADDR = 0x40;
    static const uint8_t LCD_SETDDRAMADDR = 0x80;
    
    // Entry Mode flags
    static const uint8_t LCD_ENTRYRIGHT = 0x00;
    static const uint8_t LCD_ENTRYLEFT = 0x02;
    static const uint8_t LCD_ENTRYSHIFTINCREMENT = 0x01;
    static const uint8_t LCD_ENTRYSHIFTDECREMENT = 0x00;
    
    // Display Control flags
    static const uint8_t LCD_DISPLAYON = 0x04;
    static const uint8_t LCD_DISPLAYOFF = 0x00;
    static const uint8_t LCD_CURSORON = 0x02;
    static const uint8_t LCD_CURSOROFF = 0x00;
    static const uint8_t LCD_BLINKON = 0x01;
    static const uint8_t LCD_BLINKOFF = 0x00;
    
    // Function Set flags
    static const uint8_t LCD_8BITMODE = 0x10;
    static const uint8_t LCD_4BITMODE = 0x00;
    static const uint8_t LCD_2LINE = 0x08;
    static const uint8_t LCD_1LINE = 0x00;
    static const uint8_t LCD_5x10DOTS = 0x04;
    static const uint8_t LCD_5x8DOTS = 0x00;
    
    // PCF8574 I2C Expander pins
    static const uint8_t Rs = 0x01;
    static const uint8_t Rw = 0x02;
    static const uint8_t En = 0x04;
    static const uint8_t BACKLIGHT = 0x08;
    
    // Low level functions
    void writeI2C(uint8_t data);
    void write4bits(uint8_t data);
    void expanderWrite(uint8_t data);
    void pulseEnable(uint8_t data);
    void command(uint8_t value);
    void writeChar(uint8_t value);
    void initLCD();
};

#endif
