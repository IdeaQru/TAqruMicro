#ifndef HMC5883L_H
#define HMC5883L_H

#include <Wire.h>
#include <Arduino.h>

// Address untuk QMC5883L (pengganti HMC5883L)
#define HMC5883L_ADDR 0x0D

// Register addresses
#define REG_X_LSB 0x00
#define REG_X_MSB 0x01
#define REG_Y_LSB 0x02
#define REG_Y_MSB 0x03
#define REG_Z_LSB 0x04
#define REG_Z_MSB 0x05
#define REG_STATUS 0x06
#define REG_CONFIG 0x09
#define REG_RESET 0x0A
#define REG_PERIOD 0x0B
#define REG_CHIP_ID 0x0D

class HMC5883L {
private:
    int _x, _y, _z;
    float _offsetX, _offsetY, _offsetZ;
    float _scaleX, _scaleY, _scaleZ;
    float _declination;
    
    // Tambahan untuk pitch, roll, yaw
    float _pitch, _roll, _yaw;
    float _accelX, _accelY, _accelZ; // Simulasi accelerometer
    
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void reset();
    void configure();
    String degreesToDirection(int degrees);
    
    // Fungsi tambahan untuk kalkulasi sudut
    void calculateAngles();
    float calculateTiltCompensatedHeading();

public:
    HMC5883L();
    bool begin(int sda = 21, int scl = 22);
    void read();
    
    // Getter untuk raw data
    int getX();
    int getY();
    int getZ();
    
    // Getter untuk sudut
    float getHeading();
    float getPitch();
    float getRoll();
    float getYaw();
    
    // Fungsi utility
    String getDirection(float heading);
    void calibrate(int duration = 10000);
    void setMagneticDeclination(float declination);
    
    // Fungsi untuk set data accelerometer (jika ada sensor lain)
    void setAccelData(float x, float y, float z);
    
    // Fungsi tambahan untuk QMC5883L
    bool isReady();
    uint8_t getChipID();
};

#endif
