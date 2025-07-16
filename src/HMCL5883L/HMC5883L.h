#ifndef HMC5883L_H
#define HMC5883L_H

#include <Arduino.h>
#include <Wire.h>

class HMC5883L {
public:
    HMC5883L();
    
    // Fungsi dasar
    bool begin(int sda = 21, int scl = 22);
    void read();
    
    // Getter untuk raw values
    int getX();
    int getY();
    int getZ();
    
    // Compass functions
    float getHeading();
    String getDirection(float heading);
    
    // Kalibrasi sederhana
    void calibrate(int duration = 10000);
    void setMagneticDeclination(float declination);
    
private:
    // I2C address
    static const uint8_t HMC5883L_ADDR = 0x0D;
    
    // Register addresses
    static const uint8_t REG_X_LSB = 0x00;
    static const uint8_t REG_CONFIG = 0x09;
    static const uint8_t REG_RESET = 0x0B;
    
    // Raw sensor data
    int16_t _x, _y, _z;
    
    // Kalibrasi data
    float _offsetX, _offsetY;
    float _scaleX, _scaleY;
    float _declination;
    
    // Helper functions
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void reset();
    void configure();
    String degreesToDirection(int degrees);
};

#endif
