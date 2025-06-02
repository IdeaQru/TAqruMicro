// LCDDisplay.cpp
#include "LCDDisplay.h"

LCDDisplay::LCDDisplay(int sda_pin, int scl_pin, uint8_t address) {
    sdaPin = sda_pin;
    sclPin = scl_pin;
    i2cAddress = address;
    backlightState = BACKLIGHT;
}

bool LCDDisplay::begin() {
    Wire.begin(sdaPin, sclPin);
    
    // Test I2C connection
    Wire.beginTransmission(i2cAddress);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    delay(50);
    initLCD();
    return true;
}

void LCDDisplay::initLCD() {
    // Initialize LCD in 4-bit mode
    expanderWrite(backlightState);
    delay(1000);
    
    // Put LCD into 4-bit mode
    write4bits(0x03 << 4);
    delayMicroseconds(4500);
    
    write4bits(0x03 << 4);
    delayMicroseconds(4500);
    
    write4bits(0x03 << 4);
    delayMicroseconds(150);
    
    write4bits(0x02 << 4);
    
    // Function set: 4-bit mode, 2 lines, 5x8 dots
    command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    
    // Display control: display on, cursor off, blink off
    command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    
    // Clear display
    clear();
    
    // Entry mode: left to right
    command(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
    
    delay(100);
}

void LCDDisplay::clear() {
    command(LCD_CLEARDISPLAY);
    delayMicroseconds(2000);
}

void LCDDisplay::setCursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > 3) row = 3;
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCDDisplay::print(String text) {
    for (int i = 0; i < text.length(); i++) {
        writeChar(text.charAt(i));
    }
}

void LCDDisplay::backlight(bool state) {
    if (state) {
        backlightState = BACKLIGHT;
    } else {
        backlightState = 0;
    }
    expanderWrite(0);
}

void LCDDisplay::displayMessage(String line1, String line2) {
    clear();
    setCursor(0, 0);
    print(line1);
    setCursor(0, 1);
    print(line2);
}

void LCDDisplay::displayIMUData(float roll, float pitch, float yaw) {
    clear();
    setCursor(0, 0);
    print("R:" + String(roll, 1) + " P:" + String(pitch, 1));
    setCursor(0, 1);
    print("Yaw: " + String(yaw, 1));
}

void LCDDisplay::displayGPSData(float latitude, float longitude) {
    clear();
    setCursor(0, 0);
    print("Lat:" + String(latitude, 4));
    setCursor(0, 1);
    print("Lon:" + String(longitude, 4));
}

void LCDDisplay::displayCompassData(float heading, String cardinalDir) {
    clear();
    setCursor(0, 0);
    print("Heading: " + String(heading, 1));
    setCursor(0, 1);
    print("Direction: " + cardinalDir);
}

void LCDDisplay::writeI2C(uint8_t data) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(data);
    Wire.endTransmission();
}

void LCDDisplay::expanderWrite(uint8_t data) {
    writeI2C(data | backlightState);
}

void LCDDisplay::write4bits(uint8_t data) {
    expanderWrite(data);
    pulseEnable(data);
}

void LCDDisplay::pulseEnable(uint8_t data) {
    expanderWrite(data | En);
    delayMicroseconds(1);
    
    expanderWrite(data & ~En);
    delayMicroseconds(50);
}

void LCDDisplay::command(uint8_t value) {
    write4bits(value & 0xF0);
    write4bits((value << 4) & 0xF0);
}

void LCDDisplay::writeChar(uint8_t value) {
    write4bits((value & 0xF0) | Rs);
    write4bits(((value << 4) & 0xF0) | Rs);
}
