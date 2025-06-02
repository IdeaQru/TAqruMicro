#ifndef HMC5883L_H
#define HMC5883L_H

#include <Wire.h>
#include <Arduino.h>

class HMC5883LCompass {
public:
    HMC5883LCompass();
    bool begin();
    void read();
    float getHeading() const;
    String getCardinalDirection(float heading) const;
    String getCardinalString() const; // Fungsi baru yang ditambahkan
    void calibrate(int samples = 100);
    bool isDataReady();

private:
    // HMC5883L Register addresses
    static const uint8_t HMC5883L_ADDRESS = 0x1E;
    static const uint8_t CONFIG_REG_A = 0x00;
    static const uint8_t CONFIG_REG_B = 0x01;
    static const uint8_t MODE_REG = 0x02;
    static const uint8_t DATA_OUTPUT_X_MSB = 0x03;
    static const uint8_t DATA_OUTPUT_X_LSB = 0x04;
    static const uint8_t DATA_OUTPUT_Z_MSB = 0x05;
    static const uint8_t DATA_OUTPUT_Z_LSB = 0x06;
    static const uint8_t DATA_OUTPUT_Y_MSB = 0x07;
    static const uint8_t DATA_OUTPUT_Y_LSB = 0x08;
    static const uint8_t STATUS_REG = 0x09;
    static const uint8_t ID_REG_A = 0x0A;
    static const uint8_t ID_REG_B = 0x0B;
    static const uint8_t ID_REG_C = 0x0C;
    
    // Raw magnetometer data
    int16_t rawX, rawY, rawZ;
    
    // Calibration offsets
    float offsetX, offsetY, offsetZ;
    float scaleX, scaleY, scaleZ;
    
    // Calculated heading
    float heading;
    String cardinalString; // Menyimpan string arah mata angin
    
    // Declination angle (adjust for your location)
    float declination;
    
    // I2C communication functions
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void readRawData();
    
    // Calculation functions
    float calculateHeading(float x, float y);
    void applyCalibration();
    void updateCardinalString(); // Fungsi untuk update cardinal string
};

#endif
