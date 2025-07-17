#ifndef GY25_H
#define GY25_H

#include <Wire.h>
#include <Arduino.h>

// Register addresses untuk MPU6050
#define WHO_AM_I 0x75
#define PWR_MGMT_1 0x6B
#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define INT_STATUS 0x3A
#define ACCEL_XOUT_H 0x3B

// Kemungkinan address MPU6050
#define MPU6050_ADDR_LOW 0x68   // AD0 = 0
#define MPU6050_ADDR_HIGH 0x69  // AD0 = 1

// Konstanta
#define CALIBRATION_SAMPLES 1000
#define COMPLEMENTARY_ALPHA 0.98
#define RAD_TO_DEG 57.2958

class Gy25 {
private:
    uint8_t i2cAddress;
    bool addressAutoDetected;
    
    // Raw data
    int16_t accelX, accelY, accelZ;
    int16_t gyroX, gyroY, gyroZ;
    int16_t temperature;
    
    // Calculated angles
    float roll, pitch, yaw;
    
    // Calibration offsets
    float gyroXOffset, gyroYOffset, gyroZOffset;
    float accelXOffset, accelYOffset, accelZOffset;
    
    // Scales
    float gyroScale, accelScale;
    
    // Timing
    unsigned long lastUpdate;
    
    // Private methods
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void readRawData();
    void calculateAngles();
    void updateGyroAngles(float dt);
    void complementaryFilter(float dt);
    float getAccelAngleX();
    float getAccelAngleY();
    
    // Auto-detect functions
    uint8_t autoDetectAddress();
    bool testAddress(uint8_t address);
    void scanI2CDevices();
    
public:
    Gy25(uint8_t address = 0x68);
    bool begin(int sda_pin = 21, int scl_pin = 22);
    void update();
    void calibrate();
    bool isDataReady();
    void setGyroRange(uint8_t range);
    void setAccelRange(uint8_t range);
    
    // Getters
    float getRoll() { return roll; }
    float getPitch() { return pitch; }
    float getYaw() { return yaw; }
    int16_t getAccelX() { return accelX; }
    int16_t getAccelY() { return accelY; }
    int16_t getAccelZ() { return accelZ; }
    int16_t getGyroX() { return gyroX; }
    int16_t getGyroY() { return gyroY; }
    int16_t getGyroZ() { return gyroZ; }
    int16_t getTemperature() { return temperature; }
    uint8_t getAddress() { return i2cAddress; }
    bool isAddressAutoDetected() { return addressAutoDetected; }
};

#endif
