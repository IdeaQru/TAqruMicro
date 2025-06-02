#ifndef GY25_H
#define GY25_H

#include <Wire.h>
#include <Arduino.h>

class Gy25
{
public:
    Gy25(uint8_t address = 0x68);
    bool begin(int sda_pin, int scl_pin);
    void update();
    void calibrate();

    float getRoll() const { return roll; }
    float getPitch() const { return pitch; }
    float getYaw() const { return yaw; }

    // Additional functions
    bool isDataReady();
    void setGyroRange(uint8_t range);
    void setAccelRange(uint8_t range);

private:
    uint8_t i2cAddress;
    float roll, pitch, yaw;
    unsigned long lastUpdate;

    // Raw sensor data
    int16_t accelX, accelY, accelZ;
    int16_t gyroX, gyroY, gyroZ;
    int16_t temperature;

    // Calibration offsets
    float gyroXOffset, gyroYOffset, gyroZOffset;
    float accelXOffset, accelYOffset, accelZOffset;

    // Scale factors
    float gyroScale;
    float accelScale;

    // MPU6050 Register addresses
    static const uint8_t PWR_MGMT_1 = 0x6B;
    static const uint8_t PWR_MGMT_2 = 0x6C;
    static const uint8_t SMPLRT_DIV = 0x19;
    static const uint8_t CONFIG = 0x1A;
    static const uint8_t GYRO_CONFIG = 0x1B;
    static const uint8_t ACCEL_CONFIG = 0x1C;
    static const uint8_t ACCEL_XOUT_H = 0x3B;
    static const uint8_t ACCEL_XOUT_L = 0x3C;
    static const uint8_t ACCEL_YOUT_H = 0x3D;
    static const uint8_t ACCEL_YOUT_L = 0x3E;
    static const uint8_t ACCEL_ZOUT_H = 0x3F;
    static const uint8_t ACCEL_ZOUT_L = 0x40;
    static const uint8_t TEMP_OUT_H = 0x41;
    static const uint8_t TEMP_OUT_L = 0x42;
    static const uint8_t GYRO_XOUT_H = 0x43;
    static const uint8_t GYRO_XOUT_L = 0x44;
    static const uint8_t GYRO_YOUT_H = 0x45;
    static const uint8_t GYRO_YOUT_L = 0x46;
    static const uint8_t GYRO_ZOUT_H = 0x47;
    static const uint8_t GYRO_ZOUT_L = 0x48;
    static const uint8_t WHO_AM_I = 0x75;
    static const uint8_t INT_STATUS = 0x3A;

    // Constants - Perbaikan: hapus constexpr dan gunakan static const
    static const int CALIBRATION_SAMPLES = 1000;
    const float COMPLEMENTARY_ALPHA = 0.98f;
    const float PI_VALUE = 3.14159265359f;
    // I2C communication functions
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void readRawData();

    // Calculation functions
    void calculateAngles();
    float getAccelAngleX();
    float getAccelAngleY();
    void updateGyroAngles(float dt);
    void complementaryFilter(float dt);
};

#endif
